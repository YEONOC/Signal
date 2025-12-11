// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Drone_Extract.h"
#include "SignalGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Characters/DroneCharacter.h"
#include "Item/SignalItemActor.h"
#include "SignalGameState.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_Drone_Extract::UGA_Drone_Extract()
{
	const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    AbilityInputTag = SignalTags.Input_Drone_Extract;
	AbilityTags.AddTag(SignalTags.Ability_Drone_Extract);

}

void UGA_Drone_Extract::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FHitResult HitResult;
    ASignalItemActor* TargetItem = FindItemToExtract(ActorInfo, HitResult);
    // DrawDebugExtract(ActorInfo, TargetItem);
    if (!TargetItem)
    {
        UE_LOG(LogTemp, Verbose, TEXT("GA_Drone_ExtractSignal: No valid item to extract."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CurrentTarget = TargetItem;
    CurrentExtractTime = TargetItem->GetExtractTime();
    if (CurrentExtractTime <= 0.f)
    {
        CurrentExtractTime = 0.1f; // 최소 안전값
    }

    // 채널링: ExtractTime 동안 대기
    UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, CurrentExtractTime);
    if (DelayTask)
    {
        DelayTask->OnFinish.AddDynamic(this, &UGA_Drone_Extract::OnExtractDelayFinished);
        DelayTask->ReadyForActivation();
    }

    const int32 SignalAmount = TargetItem->ExtractSignal();
    if (SignalAmount > 0)
    {
        UWorld* World = ActorInfo->AvatarActor->GetWorld();
        if (World)
        {
            if (ASignalGameState* SGS = World->GetGameState<ASignalGameState>())
            {
                SGS->AddSignal(SignalAmount);
                UE_LOG(LogTemp, Log, TEXT("GA_Drone_ExtractSignal: Extracted %d signal."), SignalAmount);
            }
        }

    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("GA_Drone_ExtractSignal: Item already extracted or zero yield."));
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Drone_Extract::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    CurrentTarget = nullptr;
    CurrentExtractTime = 0.f;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Drone_Extract::OnExtractDelayFinished()
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    ASignalItemActor* TargetItem = CurrentTarget.Get();
    if (!TargetItem)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Extract: Target lost before finish"));
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    const int32 SignalAmount = TargetItem->ExtractSignal();
    if (SignalAmount > 0)
    {
        UWorld* World = ActorInfo->AvatarActor->GetWorld();
        if (World)
        {
            if (ASignalGameState* SGS = World->GetGameState<ASignalGameState>())
            {
                SGS->AddSignal(SignalAmount);
                UE_LOG(LogTemp, Log, TEXT("GA_Drone_ExtractSignal: Extracted %d signal."), SignalAmount);
            }
        }

    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("GA_Drone_ExtractSignal: Item already extracted or zero yield."));
    }

    EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
}

ASignalItemActor* UGA_Drone_Extract::FindItemToExtract(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit) const
{
    OutHit = FHitResult();

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
    }

    AActor* Drone = ActorInfo->AvatarActor.Get();
    APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());

    FVector Start = FVector::ZeroVector;
    FVector End = FVector::ZeroVector;
    FRotator CamRot = FRotator::ZeroRotator;

    if (PC && PC->PlayerCameraManager)
    {
        // 카메라 기준으로 트레이스 (3D 게임에서 일반적인 방식)
        Start = PC->PlayerCameraManager->GetCameraLocation();
        CamRot = PC->PlayerCameraManager->GetCameraRotation();
    }
    else
    {
        // Fallback: 아바타 위치 기준
        Start = Drone->GetActorLocation();
        CamRot = Drone->GetActorRotation();
    }

    const FVector Dir = CamRot.Vector();
    End = Start + Dir * MaxExtractDistance;

    UWorld* World = Drone->GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FCollisionQueryParams Params(SCENE_QUERY_STAT(ExtractSignalTrace), false, Drone);
    Params.AddIgnoredActor(Drone);
    
    bool bHit = false;

    if (TraceRadius > 0.f)
    {
        // 스피어 트레이스 (조금 여유를 줘서 맞추기 쉽게)
        bHit = World->SweepSingleByObjectType(
            OutHit,
            Start,
            End,
            FQuat::Identity,
            ECC_SignalItem,
            FCollisionShape::MakeSphere(TraceRadius),
            Params
        );


        // Sweep 이후 디버그
#if ENABLE_DRAW_DEBUG

        const FColor HitColor = bHit ? FColor::Green : FColor::Red;
        const float LifeTime = 1.0f;   // 1초 동안 표시
        const bool  bPersistent = false;

        // 1) 드론(시작점) → End 위치까지 라인
        DrawDebugLine(
            World,
            Start,
            End,
            HitColor,
            bPersistent,
            LifeTime,
            0,
            1.5f
        );

        // 2) 실제 맞은 지점에 스피어 그리기 (못 맞췄으면 End 지점에 표시)
        const FVector SphereCenter = bHit ? OutHit.Location : End;

        DrawDebugSphere(
            World,
            SphereCenter,
            TraceRadius,
            16,
            HitColor,
            bPersistent,
            LifeTime,
            0,
            1.5f
        );

#endif
    }
    else
    {
        // 단순 라인 트레이스
        bHit = World->LineTraceSingleByObjectType(
            OutHit,
            Start,
            End,
            ECC_SignalItem,
            Params
        );
    }

    if (!bHit)
    {
        return nullptr;
    }

    AActor* HitActor = OutHit.GetActor();
    ASignalItemActor* Item = Cast<ASignalItemActor>(HitActor);

    if (!Item)
    {
        return nullptr;
    }

    DrawDebugExtract(ActorInfo, Item);

    return Item;
}

void UGA_Drone_Extract::DrawDebugExtract(const FGameplayAbilityActorInfo* ActorInfo, ASignalItemActor* Item) const
{
    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (Avatar)
    {
        const FVector DroneLoc = Avatar->GetActorLocation();
        const FVector ItemLoc = Item->GetActorLocation();

        const float Distance = FVector::Dist(DroneLoc, ItemLoc);

        const float ScanRadius = 800.f; // 혹은 GA_Drone_Scan / DataAsset / Drone에서 가져오기

        const bool bInScanRange = (Distance <= ScanRadius);

        UE_LOG(LogTemp, Warning,
            TEXT("[Extract Debug] Target=%s  Dist=%.1f  ScanRadius=%.1f  InScanRange=%s"),
            *Item->GetName(),
            Distance,
            ScanRadius,
            bInScanRange ? TEXT("TRUE") : TEXT("FALSE"));

        // 드론 기준 스캔 범위 디버그 구체
        if (UWorld* World = Avatar->GetWorld())
        {
            DrawDebugSphere(
                World,
                DroneLoc,
                ScanRadius,
                24,
                bInScanRange ? FColor::Green : FColor::Red,
                false,
                1.0f,   // 1초 동안 표시
                0,
                2.0f
            );

            // 드론 → 아이템 디버그 라인
            DrawDebugLine(
                World,
                DroneLoc,
                ItemLoc,
                FColor::Yellow,
                false,
                1.0f,
                0,
                1.5f
            );
        }
    }
}
