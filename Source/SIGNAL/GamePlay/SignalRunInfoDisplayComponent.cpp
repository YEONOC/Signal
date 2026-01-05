// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlay/SignalRunInfoDisplayComponent.h"
#include "UI/SignalRunInfoWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "World/SignalRoomBase.h"
#include "Gameplay/SignalRunSubsystem.h"
#include "SignalGameState.h"
#include "SignalRoomTypes.h"
#include "Engine/GameInstance.h"

USignalRunInfoDisplayComponent::USignalRunInfoDisplayComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Sphere Component 설정
	//SetSphereRadius(500.0f);
	//SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	/*SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);*/

	
	
}

void USignalRunInfoDisplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// Overlap 이벤트 바인딩
	OnComponentBeginOverlap.AddDynamic(this, &USignalRunInfoDisplayComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USignalRunInfoDisplayComponent::OnEndOverlap);

	// Start Room인지 확인
	if (ASignalRoomBase* Room = Cast<ASignalRoomBase>(GetOwner()))
	{
		if (Room->RoomType != ESignalRoomType::Start)
		{
#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Warning, TEXT("SignalRunInfoDisplayComponent: This component should only be used in Start Room"));
#endif
		}
	}

	// 반경 설정
	if (InteractionRadius > 0.0f)
	{
		SetSphereRadius(InteractionRadius);
	}
}

void USignalRunInfoDisplayComponent::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			OverlappedPlayerController = PC;
			ShowRunInfo();
		}
	}
}

void USignalRunInfoDisplayComponent::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			if (OverlappedPlayerController == PC)
			{
				HideRunInfo();
				OverlappedPlayerController = nullptr;
			}
		}
	}
}

void USignalRunInfoDisplayComponent::ShowRunInfo()
{
	if (!RunInfoWidgetClass)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("SignalRunInfoDisplayComponent: RunInfoWidgetClass is not set"));
#endif
		return;
	}

	if (!OverlappedPlayerController.IsValid())
	{
		return;
	}

	// 위젯이 없으면 생성
	if (!RunInfoWidget)
	{
		RunInfoWidget = CreateWidget<USignalRunInfoWidget>(OverlappedPlayerController.Get(), RunInfoWidgetClass);
		if (RunInfoWidget)
		{
			RunInfoWidget->AddToViewport();
		}
	}

	// 정보 업데이트
	UpdateRunInfo();

	// 주기적으로 업데이트 (0.5초마다)
	if (!UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&USignalRunInfoDisplayComponent::UpdateRunInfo,
			0.5f,
			true
		);
	}
}

void USignalRunInfoDisplayComponent::HideRunInfo()
{
	// 타이머 정리
	if (UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		UpdateTimerHandle.Invalidate();
	}

	// 위젯 제거
	if (RunInfoWidget)
	{
		RunInfoWidget->RemoveFromViewport();
		RunInfoWidget = nullptr;
	}
}

void USignalRunInfoDisplayComponent::UpdateRunInfo()
{
	if (!RunInfoWidget || !OverlappedPlayerController.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Run Subsystem에서 정보 가져오기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USignalRunSubsystem* RunSubsystem = GameInstance->GetSubsystem<USignalRunSubsystem>();
	if (!RunSubsystem)
	{
		return;
	}

	// GameState에서 현재 스테이지 Signal 가져오기
	ASignalGameState* GameState = World->GetGameState<ASignalGameState>();
	if (!GameState)
	{
		return;
	}

	// 정보 수집
	int32 CurrentStageIndex = RunSubsystem->GetCurrentStageIndex();
	int32 MaxStageCount = RunSubsystem->GetMaxStageCount();
	int32 CurrentStageSignal = GameState->CurrentSignal;
	int32 TotalRunSignal = RunSubsystem->GetTotalSignal();
	int32 RequiredTotalSignal = RunSubsystem->GetRequiredTotalSignal();

	// UI 업데이트
	RunInfoWidget->UpdateRunInfo(
		CurrentStageIndex,
		MaxStageCount,
		CurrentStageSignal,
		TotalRunSignal,
		RequiredTotalSignal
	);
}

