// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Drone_PassiveBattery.h"
#include "SignalGameplayTags.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Characters/DroneCharacter.h"
#include "Components/SpotLightComponent.h"

UGA_Drone_PassiveBattery::UGA_Drone_PassiveBattery()
{
    const FSignalGameplayTags& Tags = FSignalGameplayTags::Get();
    AbilityTags.AddTag(Tags.Ability_Drone_PassiveBattery);
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Drone_PassiveBattery::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("[PassiveBattery] Activated"));

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UWorld* World = ActorInfo->AvatarActor->GetWorld();
    if (!World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 1초마다 BatteryTick 호출
    World->GetTimerManager().SetTimer(
        BatteryTimerHandle,
        FTimerDelegate::CreateUObject(this, &UGA_Drone_PassiveBattery::BatteryTick, ActorInfo),
        1.0f,
        true
    );
}

void UGA_Drone_PassiveBattery::BatteryTick(const FGameplayAbilityActorInfo* ActorInfo)
{
    

    if (!ActorInfo) return;

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC) return;

    const UDroneCoreAttributeSet* Attr = ASC->GetSet<UDroneCoreAttributeSet>();
    if (!Attr) return;

    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    float Battery = Attr->GetBattery();
    float BatteryMax = Attr->GetBatteryMax();

    // 기본 drain/regen 값
    float DrainPerSec = 0.5f;  // 기본 소모
    float RegenPerSec = 0.0f;  // 기본 회복 없음

    // 라이트 켜져 있으면 추가 소모
    if (ASC->HasMatchingGameplayTag(SignalTags.State_Drone_LightOn))
    {
        DrainPerSec += 0.5f;
    }

    // 충전 중이면 drain 대신 regen
    if (ASC->HasMatchingGameplayTag(SignalTags.State_Drone_Charging))
    {
        DrainPerSec = 0.0f;
        RegenPerSec = 2.0f;
    }

    float Delta = RegenPerSec - DrainPerSec;
    if (FMath::IsNearlyZero(Delta))
    {
        return; // 변화 없으면 GE 적용 안 함
    }

    float NewBattery = FMath::Clamp(Battery + Delta, 0.0f, BatteryMax);
    float FinalDelta = NewBattery - Battery;

    if (FMath::IsNearlyZero(FinalDelta))
    {
        return;
    }

    // GE_Drone_BatteryDelta 를 SetByCaller 방식으로 적용
    ADroneCharacter* Drone = Cast<ADroneCharacter>(ActorInfo->AvatarActor.Get());
    if (!Drone || !Drone->BatteryDeltaEffect)
    {
        return;
    }

    FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
    Ctx.AddSourceObject(Drone);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Drone->BatteryDeltaEffect, 1.0f, Ctx);
    if (SpecHandle.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(SignalTags.SetByCaller_BatteryDelta, FinalDelta);
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    // 로우 배터리 태그 On/Off
    if (NewBattery <= BatteryMax * 0.2f)
    {
        ASC->AddLooseGameplayTag(SignalTags.State_Drone_LowBattery);
    }
    else
    {
        ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_LowBattery);
    }

    // 배터리 0이면 라이트 강제 0ff
    if (NewBattery <= 0.0f)
    {
        if (Drone->DroneLight && Drone->DroneLight->IsVisible())
        {
            Drone->DroneLight->SetVisibility(false);
        }

        ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_LightOn);

        // (선택) 여기서 “배터리 부족” 사운드/이펙트 트리거 가능
        UE_LOG(LogTemp, Warning, TEXT("Battery empty: Force light OFF"));
    }

}

void UGA_Drone_PassiveBattery::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        if (UWorld* World = ActorInfo->AvatarActor->GetWorld())
        {
            World->GetTimerManager().ClearTimer(BatteryTimerHandle);
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
