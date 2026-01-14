// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Drone_Scan.h"
#include "SignalGameplayTags.h"
#include "Characters/DroneCharacter.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Compo/ScanHighlightComponent.h"
#include "TimerManager.h"

UGA_Drone_Scan::UGA_Drone_Scan()
{
    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    AbilityInputTag = SignalTags.Input_Drone_Scan;
    AbilityTags.AddTag(SignalTags.Ability_Drone_Scan);
}

void UGA_Drone_Scan::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ADroneCharacter* Drone = Cast<ADroneCharacter>(ActorInfo->AvatarActor.Get());
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    if (!Drone || !ASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();
    const UDroneCoreAttributeSet* AttrSet = ASC->GetSet<UDroneCoreAttributeSet>();

    float Battery = AttrSet ? AttrSet->GetBattery() : 0.f;
    float BatteryMax = AttrSet ? AttrSet->GetBatteryMax() : 0.f;


    // 배터리 부족이면 스캔 실패
    const float MinBatteryToScan = (BatteryMax > 0.f) ? BatteryMax * 0.05f : BatteryCost;
    if (Battery < MinBatteryToScan)
    {
#if !UE_BUILD_SHIPPING
        UE_LOG(LogTemp, Warning, TEXT("Scan FAILED: Low battery (%.1f / %.1f)"), Battery, BatteryMax);
#endif
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // Blueprint의 Event ActivateAbility가 실행되도록 호출
    K2_ActivateAbility();

    // 배터리 즉시 소모
    if (Drone->BatteryDeltaEffect && AttrSet)
    {
        float NewBattery = FMath::Clamp(Battery - BatteryCost, 0.0f, BatteryMax);
        float FinalDelta = NewBattery - Battery;

        if (!FMath::IsNearlyZero(FinalDelta))
        {
            FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
            Ctx.AddSourceObject(Drone);

            FGameplayEffectSpecHandle SpecHandle =
                ASC->MakeOutgoingSpec(Drone->BatteryDeltaEffect, 1.f, Ctx);

            if (SpecHandle.IsValid())
            {
                SpecHandle.Data->SetSetByCallerMagnitude(SignalTags.SetByCaller_BatteryDelta, FinalDelta);
                ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
    }

    // 스캔 시작: 확산되는 반경으로 점진적 감지
    ScanOrigin = Drone->GetActorLocation();
    bFoundInterestingAccum = false;
    HighlightedActors.Reset();
    StartScanTick();
}

void UGA_Drone_Scan::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    StopScanTick();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Drone_Scan::StartScanTick()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    if (ScanSpeed <= 0.f)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ScanStartTime = World->GetTimeSeconds();

    // 즉시 1회 실행해서 반응성을 높임
    HandleScanTick();

    if (ScanTickInterval <= 0.f)
    {
        return;
    }

    World->GetTimerManager().SetTimer(
        ScanTickHandle,
        this,
        &UGA_Drone_Scan::HandleScanTick,
        ScanTickInterval,
        true
    );
}

void UGA_Drone_Scan::HandleScanTick()
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ADroneCharacter* Drone = Cast<ADroneCharacter>(ActorInfo->AvatarActor.Get());
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!Drone || !ASC)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    UWorld* World = Drone->GetWorld();
    if (!World)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    const float Elapsed = World->GetTimeSeconds() - ScanStartTime;
    const float CurrentRadius = FMath::Min(ScanSpeed * Elapsed, ScanRadius);

    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(CurrentRadius);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DroneScan), false, Drone);
    FCollisionObjectQueryParams ObjQuery;
    ObjQuery.AddObjectTypesToQuery(ECC_SignalItem); // Item
    ObjQuery.AddObjectTypesToQuery(ECC_SignalEnemy); // Enemy

    bool bHitSomething = World->OverlapMultiByObjectType(
        Overlaps,
        ScanOrigin,
        FQuat::Identity,
        ObjQuery,
        Sphere,
        Params
    );

    if (bHitSomething)
    {
        for (const FOverlapResult& Res : Overlaps)
        {
            AActor* HitActor = Res.GetActor();
            if (!HitActor || HitActor == Drone) continue;

            if (HighlightedActors.Contains(HitActor))
            {
                continue;
            }

            // 스캔 대상에만 아웃라인: ScanHighlightComponent가 있을 때만
            if (UScanHighlightComponent* HighlightComp = HitActor->FindComponentByClass<UScanHighlightComponent>())
            {
                HighlightComp->HighlightForScan(ScanHighlightDuration);
                HighlightedActors.Add(HitActor);

                bFoundInterestingAccum = true;
#if !UE_BUILD_SHIPPING
                UE_LOG(LogTemp, Warning, TEXT("Scan hit actor (outlined): %s"), *HitActor->GetName());
#endif
            }
        }
    }

    if (CurrentRadius >= ScanRadius)
    {
        const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();
        if (bFoundInterestingAccum)
        {
            ASC->AddLooseGameplayTag(SignalTags.State_Drone_ScanHit);
        }
        else
        {
            ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_ScanHit);
        }

        StopScanTick();
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
    }
}

void UGA_Drone_Scan::StopScanTick()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ScanTickHandle);
    }
}
