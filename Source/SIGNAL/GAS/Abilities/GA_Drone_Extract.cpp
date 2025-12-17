// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/GA_Drone_Extract.h"

#include "SignalGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "DrawDebugHelpers.h"
#include "GamePlay/DronePlayerController.h"
#include "UI/DroneHUDWidget.h"
#include "Engine/OverlapResult.h"
#include "Item/SignalItemActor.h"
#include "SignalGameState.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"

UGA_Drone_Extract::UGA_Drone_Extract()
{
	const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

	AbilityInputTag = SignalTags.Input_Drone_Extract;
	AbilityTags.AddTag(SignalTags.Ability_Drone_Extract);
}

void UGA_Drone_Extract::ActivateAbility(
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

	// 타겟 먼저 확보
	ASignalItemActor* TargetItem = FindItemToExtract(ActorInfo);
	if (!TargetItem)
	{
		if (CachedHUD)
			CachedHUD->SetInteractState(EInteractHUDState::NoTarget);

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentTarget = TargetItem;
	CurrentExtractTime = FMath::Max(TargetItem->GetExtractTime(), 0.1f);

	// HUD 시작 상태
	if (CachedHUD)
	{
		CachedHUD->SetInteractState(EInteractHUDState::Extracting);
		CachedHUD->SetInteractProgress(0.f);
	}

	ExtractStartTime = GetWorld()->GetTimeSeconds();

	// 진행률 타이머
	GetWorld()->GetTimerManager().SetTimer(
		ProgressTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (!IsActive())
					return;

				if (!CachedHUD || !CurrentTarget.IsValid())
					return;

				const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
				if (!Info || !Info->AvatarActor.IsValid())
					return;

				if (bCancelRequested)
					return;

				AActor* Drone = Info->AvatarActor.Get();

				const float Dist = FVector::Dist(Drone->GetActorLocation(), CurrentTarget->GetActorLocation());
				
				// 거리 이탈 시 Cancelled
				if (Dist > ExtractRadius)
				{
					bCancelRequested = true;

					CachedHUD->SetInteractState(EInteractHUDState::Cancelled);

					if (UWorld* World = GetWorld())
					{
						World->GetTimerManager().ClearTimer(ProgressTimerHandle);
					}

					CancelAbility(CurrentSpecHandle, Info, CurrentActivationInfo, true);
					return;
				}

				const float Now = GetWorld()->GetTimeSeconds();
				const float Alpha = FMath::Clamp((Now - ExtractStartTime) / CurrentExtractTime, 0.f, 1.f);

				CachedHUD->SetInteractProgress(Alpha);
			}),
		0.05f,
		true
	);

	// 채널링 완료 대기
	ActiveDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, CurrentExtractTime);
	if (!ActiveDelayTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveDelayTask->OnFinish.AddDynamic(this, &UGA_Drone_Extract::OnExtractDelayFinished);
	ActiveDelayTask->ReadyForActivation();

}

void UGA_Drone_Extract::OnExtractDelayFinished()
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		if (CachedHUD)
			CachedHUD->SetInteractState(EInteractHUDState::Cancelled);

		EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AActor* Drone = ActorInfo->AvatarActor.Get();
	ASignalItemActor* TargetItem = CurrentTarget.Get();

	if (!TargetItem)
	{
		if (CachedHUD)
			CachedHUD->SetInteractState(EInteractHUDState::NoTarget);

		EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 최종 거리 검증
	const float Dist = FVector::Dist(Drone->GetActorLocation(), TargetItem->GetActorLocation());
	if (Dist > ExtractRadius)
	{
		if (CachedHUD)
			CachedHUD->SetInteractState(EInteractHUDState::Cancelled);

		EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 실제 추출
	const int32 SignalAmount = TargetItem->ExtractSignal();
	if (SignalAmount > 0)
	{
		if (UWorld* World = Drone->GetWorld())
		{
			if (ASignalGameState* SGS = World->GetGameState<ASignalGameState>())
			{
				SGS->AddSignal(SignalAmount);
			}
		}
	}

	if (CachedHUD)
	{
		CachedHUD->SetInteractProgress(1.f);
		CachedHUD->SetInteractState(EInteractHUDState::Completed);
	}

	EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Drone_Extract::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
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
		CachedHUD->SetInteractState(EInteractHUDState::Cancelled);
		CachedHUD->SetInteractProgress(0.f);
	}

	CachedHUD = nullptr;
	CurrentTarget = nullptr;
	CurrentExtractTime = 0.f;
	ExtractStartTime = 0.f;
	bCancelRequested = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ASignalItemActor* UGA_Drone_Extract::FindItemToExtract(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
		return nullptr;

	AActor* Drone = ActorInfo->AvatarActor.Get();
	UWorld* World = Drone->GetWorld();
	if (!World)
		return nullptr;

	// 카메라 기준
	FVector CamLoc = Drone->GetActorLocation();
	FRotator CamRot = Drone->GetActorRotation();

	if (ADronePlayerController* PC = Cast<ADronePlayerController>(ActorInfo->PlayerController.Get()))
	{
		if (PC->PlayerCameraManager)
		{
			CamLoc = PC->PlayerCameraManager->GetCameraLocation();
			CamRot = PC->PlayerCameraManager->GetCameraRotation();
		}
	}

	const FVector CamForward = CamRot.Vector();
	const FVector Center = Drone->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ExtractOverlap), false, Drone);
	Params.AddIgnoredActor(Drone);

	const bool bAny = World->OverlapMultiByObjectType(
		Overlaps,
		Center,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_SignalItem),
		FCollisionShape::MakeSphere(ExtractRadius),
		Params
	);

	if (!bAny)
		return nullptr;

	ASignalItemActor* BestItem = nullptr;
	float BestScore = -FLT_MAX;

	for (const FOverlapResult& R : Overlaps)
	{
		ASignalItemActor* Item = Cast<ASignalItemActor>(R.GetActor());
		if (!Item)
			continue;

		const FVector ToItem = Item->GetActorLocation() - CamLoc;
		const float Dist = ToItem.Length();
		if (Dist <= KINDA_SMALL_NUMBER)
			continue;

		const FVector Dir = ToItem / Dist;
		const float FacingDot = FVector::DotProduct(CamForward, Dir);
		if (FacingDot < MinFacingDot)
			continue;

		const float DistAlpha = 1.f - FMath::Clamp(Dist / (ExtractRadius * 2.f), 0.f, 1.f);
		const float Score = FacingDot + DistAlpha * 0.5f;

		if (Score > BestScore)
		{
			BestScore = Score;
			BestItem = Item;
		}
	}

	return BestItem;
}
