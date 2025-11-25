// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "GA_Drone_PassiveBattery.generated.h"

/**
 * 
 */
UCLASS()
class SIGNAL_API UGA_Drone_PassiveBattery : public USignalGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Drone_PassiveBattery();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled
    ) override;

private:
    FTimerHandle BatteryTimerHandle;

    void BatteryTick(const FGameplayAbilityActorInfo* ActorInfo);
};
