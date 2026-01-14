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

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled
    ) override;

protected:
    // 스캔 반경, 배터리 소모량 등
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
    float ScanRadius = 800.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
    float BatteryCost = 5.f;

    // 스캔 확산 속도 (cm/s)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
    float ScanSpeed = 1500.f;

    // 스캔 확산 업데이트 간격
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
    float ScanTickInterval = 0.05f;

    // 스캔 아이템 Highlight Duration
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
    float ScanHighlightDuration = 2.f;

private:
    void StartScanTick();
    void HandleScanTick();
    void StopScanTick();

    FTimerHandle ScanTickHandle;
    float ScanStartTime = 0.f;
    FVector ScanOrigin = FVector::ZeroVector;
    bool bFoundInterestingAccum = false;
    TSet<TWeakObjectPtr<AActor>> HighlightedActors;
};
