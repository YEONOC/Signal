// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "TimerManager.h"
#include "GA_Drone_Extract.generated.h"

#define ECC_SignalItem ECollisionChannel::ECC_GameTraceChannel1

class ASignalItemActor;
class UAbilityTask_WaitDelay;
class UAbilityTask_WaitInputRelease;
class UDroneHUDWidget;

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
    // 채널링 후 추출 완료
    UFUNCTION()
    void OnExtractDelayFinished();

    // 아이템 찾는 트레이스
    ASignalItemActor* FindItemToExtract(const FGameplayAbilityActorInfo* ActorInfo) const;

protected:
    // 드론 기준 추출 가능 반경
    UPROPERTY(EditDefaultsOnly, Category = "Signal|Extract")
    float ExtractRadius = 150.f;

    // 카메라가 대략이라도 바라보는 대상만 허용(dot)
    UPROPERTY(EditDefaultsOnly, Category = "Signal|Extract")
    float MinFacingDot = 0.2f;

    UPROPERTY()
    TObjectPtr<UDroneHUDWidget> CachedHUD = nullptr;

    FTimerHandle ProgressTimerHandle;

    UPROPERTY()
    float ExtractStartTime = 0.f;

    // (선택) 디버그 드로우
    UPROPERTY(EditDefaultsOnly, Category = "Signal|Extract")
    bool bDrawDebug = true;

    bool bCancelRequested = false;

protected:
    // 이번 채널링 동안 타겟으로 잡힌 아이템
    UPROPERTY()
    TWeakObjectPtr<ASignalItemActor> CurrentTarget;

    // 캐싱된 ExtractTime (아이템에서 읽어옴)
    UPROPERTY()
    float CurrentExtractTime = 0.f;

    // 활성 딜레이 태스크
    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitDelay> ActiveDelayTask = nullptr;
};
