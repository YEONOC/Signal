// Copyright Epic Games, Inc. All Rights Reserved.

#include "SignalGameMode.h"
#include "Gameplay/SignalRunSubsystem.h"
#include "World/SignalFacilityGenerator.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "SignalGameState.h"

ASIGNALGameMode::ASIGNALGameMode()
{
    bStartPlayersAsSpectators = true;
}

void ASIGNALGameMode::BeginPlay()
{
    Super::BeginPlay();

    RunSubsystem = GetGameInstance()->GetSubsystem<USignalRunSubsystem>();
    check(RunSubsystem);

    if (!FacilityGenerator)
    {
        for (TActorIterator<ASignalFacilityGenerator> It(GetWorld()); It; ++It)
        {
            FacilityGenerator = *It;
            break;
        }
    }

    checkf(FacilityGenerator, TEXT("FacilityGenerator is not assigned in GameMode"));

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
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Log, TEXT("Start Stage %d | Seed=%d"), RunSubsystem->GetCurrentStageIndex(), StageSeed);
#endif
    // 2. 스테이지 생성 구독 && Procedural Generation 호출
    FacilityGenerator->OnStageGenerated.AddDynamic(this, &ASIGNALGameMode::HandleStageGenerated);
    FacilityGenerator->GenerateStage(StageSeed);

    // 3. StageConfig 선택
    const int32 StageIndex = RunSubsystem->GetCurrentStageIndex();
    if (StageConfigs.IsValidIndex(StageIndex - 1))
    {
        GS->StageConfig = StageConfigs[StageIndex - 1];
    }
    else
    {
#if !UE_BUILD_SHIPPING
        UE_LOG(LogTemp, Warning, TEXT("Invalid StageConfig index: %d"), StageIndex);
#endif
    }

    // 4. GameState 초기화
    GS->InitializeSignalFromStage();

    // 5. Stage Cleared 이벤트 구독
    GS->OnStageCleared.RemoveDynamic(this, &ASIGNALGameMode::HandleStageCleared);
    GS->OnStageCleared.AddDynamic(this, &ASIGNALGameMode::HandleStageCleared);

}

void ASIGNALGameMode::HandleStageGenerated(const FTransform& StartTransform)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    // StartTransform 위치에 스폰/리스폰
    RestartPlayerAtTransform(PC, StartTransform);

    // 중복 방지
    FacilityGenerator->OnStageGenerated.RemoveDynamic(this, &ASIGNALGameMode::HandleStageGenerated);
}

void ASIGNALGameMode::HandleStageCleared()
{
    ASignalGameState* GS = GetGameState<ASignalGameState>();
    if (!GS) return;

    const int32 StageSignal = GS->CurrentSignal;

    // 이벤트 중복 방지
    GS->OnStageCleared.RemoveDynamic(this, &ASIGNALGameMode::HandleStageCleared);

    FacilityGenerator->ClearGeneratedActors();

    RunSubsystem->FinishCurrentStage(StageSignal);

    if (RunSubsystem->HasRemainingStages())
    {
        StartStage();
    }
}

void ASIGNALGameMode::HandleStageFailed()
{
    RunSubsystem->FailCurrentStage();

    FacilityGenerator->ClearGeneratedActors();

    if (RunSubsystem->HasRemainingStages())
    {
        StartStage();
    }
}

void ASIGNALGameMode::EndStage(bool bSuccess)
{
    ASignalGameState* GS = GetGameState<ASignalGameState>();
    if (!GS || !RunSubsystem) return;

    const int32 StageSignal = GS->CurrentSignal; // Stage에서 추출한 시그널

    // 탈출에 성공 (복귀 성공)
    if (bSuccess)
    {
        RunSubsystem->FinishCurrentStage(StageSignal);
    }
    // 탈출 실패 (배터리 방전 || etc)
    else
    {
        RunSubsystem->FailCurrentStage();
    }

    // 해당 Run에 잔여 스테이지가 남아있으면 다음 스테이지 시작
    if (RunSubsystem->HasRemainingStages())
    {
        StartStage();
    }
}

void ASIGNALGameMode::HandleRunCleared()
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Log, TEXT("Run Cleared!"));
#endif
    // 결과 UI
    // 다음 Run 준비
}

void ASIGNALGameMode::HandleRunFailed()
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning, TEXT("Run Failed"));
#endif
    // 결과 UI
    // 재시작 or 메인 메뉴
}