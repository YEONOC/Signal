// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "GA_Drone_Extract.generated.h"

#define ECC_SignalItem ECollisionChannel::ECC_GameTraceChannel1

class ASignalItemActor;

/**
 * 
 */
UCLASS()
class SIGNAL_API UGA_Drone_Extract : public USignalGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_Drone_Extract();

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
    /** 채널링 후 추출 완료 */
    UFUNCTION()
    void OnExtractDelayFinished();

    /** 아이템 찾는 트레이스 */
    ASignalItemActor* FindItemToExtract(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit) const;

    void DrawDebugExtract(const FGameplayAbilityActorInfo* ActorInfo, ASignalItemActor* Item) const;

protected:
    /** 최대 추출 거리 (라인 트레이스 거리) */
    UPROPERTY(EditDefaultsOnly, Category = "Signal|Extract")
    float MaxExtractDistance = 500.f;

    /** 약간 여유 있는 스피어 반경 (라인이 살짝 빗나가도 잡아주기 위함) */
    UPROPERTY(EditDefaultsOnly, Category = "Signal|Extract")
    float TraceRadius = 30.f;

    // 이번 채널링 동안 타겟으로 잡힌 아이템
    UPROPERTY()
    TWeakObjectPtr<ASignalItemActor> CurrentTarget;

    // 캐싱된 ExtractTime (아이템에서 읽어옴)
    UPROPERTY()
    float CurrentExtractTime = 0.f;
};
