// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Drone_LightToggle.h"
#include "Characters/DroneCharacter.h"
#include "SignalGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Components/SpotLightComponent.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"

UGA_Drone_LightToggle::UGA_Drone_LightToggle()
{
    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    UE_LOG(LogTemp, Warning, TEXT("LightToggle ctor Tag valid? %s"),
        SignalTags.Ability_Drone_LightToggle.IsValid() ? TEXT("true") : TEXT("false"));

    AbilityInputTag = SignalTags.Ability_Drone_LightToggle;
    AbilityTags.AddTag(SignalTags.Ability_Drone_LightToggle);
}

void UGA_Drone_LightToggle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("LightToggle Ability Activated!"));
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ADroneCharacter* Drone = Cast<ADroneCharacter>(ActorInfo->AvatarActor.Get());
    if (!Drone || !Drone->DroneLight)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    const UDroneCoreAttributeSet* AttrSet = ASC->GetSet<UDroneCoreAttributeSet>();
    float Battery = AttrSet ? AttrSet->GetBattery() : 0.f;
    float BatteryMax = AttrSet ? AttrSet->GetBatteryMax() : 0.f;

    const bool bIsOn = Drone->DroneLight->IsVisible();

    if (bIsOn)
    {
        // Light OFF
        Drone->DroneLight->SetVisibility(false);
        ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_LightOn);
    }
    else
    {
        // 꺼져있는 상태에서 "켜려는" 경우 → 배터리 체크

        const float MinBatteryToTurnOn = (BatteryMax > 0.f) ? BatteryMax * 0.05f : 5.f; // 전체의 5% 정도

        if (Battery <= 0.f || Battery < MinBatteryToTurnOn)
        {
            // 배터리 부족 → 켜기 실패
            UE_LOG(LogTemp, Warning, TEXT("Cannot turn light ON: Low battery (%.1f / %.1f)"), Battery, BatteryMax);

            // (선택) 여기서 HUD에 경고 표시, 사운드, 깜빡임 등
        }
        else
        {
            // 배터리 충분 → ON
            Drone->DroneLight->SetVisibility(true);
            ASC->AddLooseGameplayTag(SignalTags.State_Drone_LightOn);
        }

    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
