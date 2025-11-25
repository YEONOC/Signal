// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Drone_Scan.h"
#include "SignalGameplayTags.h"
#include "Characters/DroneCharacter.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Compo/ScanHighlightComponent.h"

UGA_Drone_Scan::UGA_Drone_Scan()
{
    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    AbilityInputTag = SignalTags.Ability_Drone_Scan;
    AbilityTags.AddTag(SignalTags.Ability_Drone_Scan);

    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
        UE_LOG(LogTemp, Warning, TEXT("Scan FAILED: Low battery (%.1f / %.1f)"), Battery, BatteryMax);
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }


    // 스캔 로직: 구체 트레이스
    FVector Origin = Drone->GetActorLocation();
    UWorld* World = Drone->GetWorld();
    if (!World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(ScanRadius);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DroneScan), false, Drone);

    bool bHitSomething = World->OverlapMultiByChannel(
        Overlaps,
        Origin,
        FQuat::Identity,
        ECC_Pawn,      // 필요한 채널로 변경 가능
        Sphere,
        Params
    );

    bool bFoundInteresting = false;

    if (bHitSomething)
    {
        for (const FOverlapResult& Res : Overlaps)
        {
            AActor* HitActor = Res.GetActor();
            if (!HitActor || HitActor == Drone) continue;

            // 스캔 대상에만 아웃라인: ScanHighlightComponent가 있을 때만
            if (UScanHighlightComponent* HighlightComp = HitActor->FindComponentByClass<UScanHighlightComponent>())
            {
                // HazardLevel 읽어오기
                int32 Hazard = HighlightComp->HazardLevel;

                HighlightComp->HighlightForScan(2.0f);

                bFoundInteresting = true;

                UE_LOG(LogTemp, Warning, TEXT("Scan hit actor (outlined): %s"), *HitActor->GetName());
            }

            // 디버그용 스피어
            DrawDebugSphere(World, HitActor->GetActorLocation(), 50.f, 12, FColor::Green, false, 1.0f);
        }
    }

    // 디버그: 전체 스캔 범위 표시
    DrawDebugSphere(World, Origin, ScanRadius, 32,
        bFoundInteresting ? FColor::Green : FColor::Red,
        false, 1.0f);

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

    // 감지 성공 시 태그 부여 (나중에 UI 등에 사용 가능)
    if (bFoundInteresting)
    {
        ASC->AddLooseGameplayTag(SignalTags.State_Drone_ScanHit);
    }
    else
    {
        ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_ScanHit);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
