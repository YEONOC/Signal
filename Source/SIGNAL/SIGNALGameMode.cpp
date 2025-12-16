// Copyright Epic Games, Inc. All Rights Reserved.

#include "SignalGameMode.h"
#include "Gameplay/SignalRunSubsystem.h"
#include "SignalGameState.h"

void ASIGNALGameMode::BeginPlay()
{
    Super::BeginPlay();

    RunSubsystem = GetGameInstance()->GetSubsystem<USignalRunSubsystem>();
    check(RunSubsystem);

    // Run 종료 이벤트 구독
    RunSubsystem->OnRunCleared.AddDynamic(this, &ASIGNALGameMode::HandleRunCleared);
    RunSubsystem->OnRunFailed.AddDynamic(this, &ASIGNALGameMode::HandleRunFailed);

    // Run 시작
    RunSubsystem->StartNewRun(RunConfig);

    StartStage();
}

void ASIGNALGameMode::StartStage()
{
    ASignalGameState* GS = GetGameState<ASignalGameState>();
    check(GS);

    // 1. StageSeed 받기
    const int32 StageSeed = RunSubsystem->GetCurrentStageSeed();

    // 2. Procedural Generation 호출
    // 예: FacilityGenerator->Generate(StageSeed);

    // 3. StageConfig 선택
    const int32 StageIndex = RunSubsystem->GetCurrentStageIndex();
    if (StageConfigs.IsValidIndex(StageIndex - 1))
    {
        GS->StageConfig = StageConfigs[StageIndex - 1];
    }

    // 4. GameState 초기화
    GS->InitializeSignalFromStage();

    // 5. Stage Cleared 이벤트 구독
    GS->OnStageCleared.AddDynamic(this, &ASIGNALGameMode::HandleStageCleared);
}

void ASIGNALGameMode::HandleStageCleared()
{
    ASignalGameState* GS = GetGameState<ASignalGameState>();
    if (!GS) return;

    const int32 StageSignal = GS->CurrentSignal;

    // 이벤트 중복 방지
    GS->OnStageCleared.RemoveDynamic(this, &ASIGNALGameMode::HandleStageCleared);

    RunSubsystem->FinishCurrentStage(StageSignal);

    if (RunSubsystem->HasRemainingStages())
    {
        StartStage();
    }
}

void ASIGNALGameMode::HandleRunCleared()
{
    UE_LOG(LogTemp, Log, TEXT("Run Cleared!"));

    // 결과 UI
    // 다음 Run 준비
}

void ASIGNALGameMode::HandleRunFailed()
{
    UE_LOG(LogTemp, Warning, TEXT("Run Failed"));

    // 결과 UI
    // 재시작 or 메인 메뉴
}