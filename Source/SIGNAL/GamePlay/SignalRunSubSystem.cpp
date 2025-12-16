// SignalRunSubsystem.cpp
#include "SignalRunSubsystem.h"
#include "SignalRunConfig.h"

void USignalRunSubsystem::StartNewRun(USignalRunConfig* InConfig)
{
    check(InConfig);

    RunConfig = InConfig;

    RunSeed = FMath::Rand();
    CurrentStageIndex = 0;
    TotalCollectedSignal = 0;

    AdvanceStage();
}

void USignalRunSubsystem::FinishCurrentStage(int32 StageSignal)
{
    TotalCollectedSignal += StageSignal;

    if (!HasRemainingStages())
    {
        if (IsRunCleared())
        {
            OnRunCleared.Broadcast();
        }
        else
        {
            OnRunFailed.Broadcast();
        }
        return;
    }

    AdvanceStage();
}

void USignalRunSubsystem::FailCurrentStage()
{
    if (!HasRemainingStages())
    {
        OnRunFailed.Broadcast();
        return;
    }

    AdvanceStage();
}

void USignalRunSubsystem::AdvanceStage()
{
    CurrentStageIndex++;
    OnStageIndexChanged.Broadcast(CurrentStageIndex);
}

bool USignalRunSubsystem::HasRemainingStages() const
{
    return RunConfig && CurrentStageIndex < RunConfig->MaxStageCount;
}

bool USignalRunSubsystem::IsRunCleared() const
{
    return RunConfig && TotalCollectedSignal >= RunConfig->RequiredTotalSignal;
}

bool USignalRunSubsystem::IsRunFailed() const
{
    return !IsRunCleared() && !HasRemainingStages();
}

int32 USignalRunSubsystem::GetCurrentStageIndex() const
{
    return CurrentStageIndex;
}

int32 USignalRunSubsystem::GetMaxStageCount() const
{
    return RunConfig ? RunConfig->MaxStageCount : 0;
}

int32 USignalRunSubsystem::GetTotalSignal() const
{
    return TotalCollectedSignal;
}

int32 USignalRunSubsystem::GetRunSeed() const
{
    return RunSeed;
}

int32 USignalRunSubsystem::GetCurrentStageSeed() const
{
    return RunSeed + CurrentStageIndex * 1000;
}
