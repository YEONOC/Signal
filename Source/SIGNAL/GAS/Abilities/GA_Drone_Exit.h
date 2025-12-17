// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "TimerManager.h"
#include "GA_Drone_Exit.generated.h"

class UAbilityTask_WaitDelay;
class ASignalExitActor;
class UDroneHUDWidget;

/**
 * 
 */
UCLASS()
class SIGNAL_API UGA_Drone_Exit : public USignalGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Drone_Exit();

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
	// 채널링 후 Exit 완료
	UFUNCTION()
	void OnExitDelayFinished();

protected:
	// 드론 기준 탈출 시도 가능 반경
	UPROPERTY(EditDefaultsOnly, Category = "Signal|Exit")
	float ExitRadius = 150.f;

	UPROPERTY()
	TWeakObjectPtr<ASignalExitActor> CurrentExit;

	// 탈출 Hold 시간
	UPROPERTY(EditDefaultsOnly, Category = "Signal|Exit")
	float ExitHoldTime = 2.f;

	// 탈출 시작 시간
	UPROPERTY()
	float ExitStartTime = 0.f;

	UPROPERTY()
	TObjectPtr<UDroneHUDWidget> CachedHUD = nullptr;

	// 활성 딜레이 태스크
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> ActiveDelayTask = nullptr;

protected:
	bool bCancelRequested = false;

	FTimerHandle ProgressTimerHandle;

	
};
