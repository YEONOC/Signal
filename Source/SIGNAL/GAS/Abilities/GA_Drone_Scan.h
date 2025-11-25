// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "GA_Drone_Scan.generated.h"

/**
 * 
 */
UCLASS()
class SIGNAL_API UGA_Drone_Scan : public USignalGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Drone_Scan();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    // 스캔 반경, 배터리 소모량 등
    UPROPERTY(EditDefaultsOnly, Category = "Scan")
    float ScanRadius = 800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Scan")
    float BatteryCost = 5.f;
};
