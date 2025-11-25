// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Drone_LightToggle.h"
#include "Characters/DroneCharacter.h"
#include "SignalGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Components/SpotLightComponent.h"

UGA_Drone_LightToggle::UGA_Drone_LightToggle()
{
    const FSignalGameplayTags& Tags = FSignalGameplayTags::Get();

    UE_LOG(LogTemp, Warning, TEXT("LightToggle ctor Tag valid? %s"),
        Tags.Ability_Drone_LightToggle.IsValid() ? TEXT("true") : TEXT("false"));

    AbilityInputTag = Tags.Ability_Drone_LightToggle;
    AbilityTags.AddTag(Tags.Ability_Drone_LightToggle);
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

    const FSignalGameplayTags& Tags = FSignalGameplayTags::Get();

    const bool bIsOn = Drone->DroneLight->IsVisible();

    if (bIsOn)
    {
        // 🔻 Light OFF
        Drone->DroneLight->SetVisibility(false);
        ASC->RemoveLooseGameplayTag(Tags.State_Drone_LightOn);
    }
    else
    {
        // 🔺 Light ON
        Drone->DroneLight->SetVisibility(true);
        ASC->AddLooseGameplayTag(Tags.State_Drone_LightOn);

    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
