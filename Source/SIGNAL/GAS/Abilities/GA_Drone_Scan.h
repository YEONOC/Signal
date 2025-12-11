// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "GA_Drone_Scan.generated.h"

#define ECC_SignalItem ECollisionChannel::ECC_GameTraceChannel1
#define ECC_SignalEnemy ECollisionChannel::ECC_GameTraceChannel2

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

protected:
    // 스캔 반경, 배터리 소모량 등
    UPROPERTY(EditDefaultsOnly, Category = "Scan")
    float ScanRadius = 800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Scan")
    float BatteryCost = 5.f;
};
