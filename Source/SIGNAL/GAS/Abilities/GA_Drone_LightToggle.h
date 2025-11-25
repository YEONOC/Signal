// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignalGameplayAbility.h"
#include "GA_Drone_LightToggle.generated.h"

UCLASS()
class SIGNAL_API UGA_Drone_LightToggle : public USignalGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Drone_LightToggle();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

