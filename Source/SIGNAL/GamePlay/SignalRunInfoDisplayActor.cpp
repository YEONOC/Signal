// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlay/SignalRunInfoDisplayActor.h"
#include "UI/SignalRunInfoWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Gameplay/SignalRunSubsystem.h"
#include "SignalGameState.h"
#include "Engine/GameInstance.h"

ASignalRunInfoDisplayActor::ASignalRunInfoDisplayActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component 생성
	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	// Widget Component 생성
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	// DisplaySize는 BeginPlay에서 설정 (생성자에서는 기본값 사용)
	WidgetComponent->SetDrawSize(FIntPoint(1920, 1080)); // 기본값
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetVisibility(false); // 기본적으로 숨김

	// Trigger Sphere 생성
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(RootComponent);
	// InteractionRadius는 BeginPlay에서 설정 (생성자에서는 기본값 사용)
	TriggerSphere->SetSphereRadius(1000.0f); // 기본값
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	// Overlap 이벤트 바인딩
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASignalRunInfoDisplayActor::OnBeginOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ASignalRunInfoDisplayActor::OnEndOverlap);
}

void ASignalRunInfoDisplayActor::BeginPlay()
{
	Super::BeginPlay();

	// 반경 설정
	if (InteractionRadius > 0.0f)
	{
		TriggerSphere->SetSphereRadius(InteractionRadius);
	}

	// 디스플레이 크기 설정
	WidgetComponent->SetDrawSize(DisplaySize);

	// 위젯 생성
	if (RunInfoWidgetClass)
	{
		RunInfoWidget = CreateWidget<USignalRunInfoWidget>(GetWorld(), RunInfoWidgetClass);
		if (RunInfoWidget)
		{
			WidgetComponent->SetWidget(RunInfoWidget);
		}
	}

	// 항상 표시 모드면 바로 표시
	if (bAlwaysVisible)
	{
		WidgetComponent->SetVisibility(true);
		UpdateRunInfo();

		// 주기적으로 업데이트
		if (!UpdateTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(
				UpdateTimerHandle,
				this,
				&ASignalRunInfoDisplayActor::UpdateRunInfo,
				0.5f,
				true
			);
		}
	}
}

void ASignalRunInfoDisplayActor::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bAlwaysVisible)
	{
		return; // 이미 표시 중이면 무시
	}

	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			OverlappedPlayerController = PC;
			ShowRunInfo();
		}
	}
}

void ASignalRunInfoDisplayActor::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (bAlwaysVisible)
	{
		return; // 항상 표시 모드면 무시
	}

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

void ASignalRunInfoDisplayActor::ShowRunInfo()
{
	if (!RunInfoWidget)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("SignalRunInfoDisplayActor: RunInfoWidget is not created"));
#endif
		return;
	}

	// WidgetComponent 표시
	WidgetComponent->SetVisibility(true);

	// 정보 업데이트
	UpdateRunInfo();

	// 주기적으로 업데이트 (0.5초마다)
	if (!UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&ASignalRunInfoDisplayActor::UpdateRunInfo,
			0.5f,
			true
		);
	}
}

void ASignalRunInfoDisplayActor::HideRunInfo()
{
	// 타이머 정리
	if (UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		UpdateTimerHandle.Invalidate();
	}

	// WidgetComponent 숨김
	WidgetComponent->SetVisibility(false);
}

void ASignalRunInfoDisplayActor::UpdateRunInfo()
{
	if (!RunInfoWidget)
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

