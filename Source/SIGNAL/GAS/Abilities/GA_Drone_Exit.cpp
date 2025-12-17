// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Drone_Exit.h"

#include "SignalGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GamePlay/DronePlayerController.h"
#include "GamePlay/SignalExitActor.h"
#include "Characters/DroneCharacter.h"
#include "UI/DroneHUDWidget.h"

UGA_Drone_Exit::UGA_Drone_Exit()
{
	const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

	AbilityInputTag = SignalTags.Input_Drone_Exit;
	AbilityTags.AddTag(SignalTags.Ability_Drone_Exit);
}

void UGA_Drone_Exit::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() ||
		!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 중복 방지
	if (ActiveDelayTask)
	{
		ActiveDelayTask->EndTask();
		ActiveDelayTask = nullptr;
	}

	// HUD 캐싱
	ADronePlayerController* PC = Cast<ADronePlayerController>(ActorInfo->PlayerController.Get());
	CachedHUD = PC ? PC->GetDroneHUD() : nullptr;

	// 탈출 오브젝트 확보
	ADroneCharacter* Drone = Cast<ADroneCharacter>(ActorInfo->AvatarActor.Get());
	if (!Drone)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASignalExitActor* ExitActor = Drone->GetCurrentExit();
	if (!ExitActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentExit = ExitActor;

	// Wait Delay
	if (CachedHUD)
	{
		CachedHUD->SetInteractProgress(0.f);
	}

	ExitHoldTime = FMath::Max(ExitHoldTime, 0.1f);
	ExitStartTime = GetWorld()->GetTimeSeconds();
	
	// 진행률 타이머
	GetWorld()->GetTimerManager().SetTimer(
		ProgressTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (!IsActive())
					return;

				if (!CachedHUD || !CurrentExit.IsValid())
					return;

				const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
				if (!Info || !Info->AvatarActor.IsValid())
					return;

				if (bCancelRequested)
					return;

				AActor* Drone = Info->AvatarActor.Get();

				const float Dist = FVector::Dist(Drone->GetActorLocation(), CurrentExit->GetActorLocation());

				// 거리 이탈 시 Cancelled
				if (Dist > ExitRadius)
				{
					bCancelRequested = true;

					if (UWorld* World = GetWorld())
					{
						World->GetTimerManager().ClearTimer(ProgressTimerHandle);
					}

					CancelAbility(CurrentSpecHandle, Info, CurrentActivationInfo, true);
					return;
				}

				const float Now = GetWorld()->GetTimeSeconds();
				const float Alpha = FMath::Clamp((Now - ExitStartTime) / ExitHoldTime, 0.f, 1.f);

				CachedHUD->SetInteractProgress(Alpha);
			}),
		0.05f,
		true
	);

	// 채널링 완료 대기
	ActiveDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ExitHoldTime);
	if (!ActiveDelayTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveDelayTask->OnFinish.AddDynamic(this, &UGA_Drone_Exit::OnExitDelayFinished);
	ActiveDelayTask->ReadyForActivation();
}

void UGA_Drone_Exit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 진행률 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	if (ActiveDelayTask)
	{
		ActiveDelayTask->EndTask();
		ActiveDelayTask = nullptr;
	}

	if (bWasCancelled && CachedHUD)
	{
		CachedHUD->SetInteractProgress(0.f);
	}

	CachedHUD = nullptr;
	CurrentExit = nullptr;
	ExitStartTime = 0.f;
	bCancelRequested = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Drone_Exit::OnExitDelayFinished()
{
	if (!IsActive())
		return;

	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	if (!Info || !Info->AvatarActor.IsValid())
	{
		EndAbility(CurrentSpecHandle, Info, CurrentActivationInfo, true, true);
		return;
	}

	if (!CurrentExit.IsValid())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AActor* Avatar = Info->AvatarActor.Get();
	const float Dist = FVector::Dist(Avatar->GetActorLocation(), CurrentExit->GetActorLocation());
	if (Dist > ExitRadius)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CachedHUD)
	{
		CachedHUD->SetInteractProgress(1.f);
	}

	CurrentExit->TryExit();

	EndAbility(CurrentSpecHandle, Info, CurrentActivationInfo, true, false);
}
