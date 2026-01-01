// SignalRunSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SignalRunSubsystem.generated.h"

class USignalRunConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageIndexChanged, int32, NewStageIndex);

UCLASS()
class SIGNAL_API USignalRunSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /* Run 제어 */
    void StartNewRun(USignalRunConfig* InConfig);
    void FinishCurrentStage(int32 StageSignal);
    void FailCurrentStage();

    /* 상태 조회 */
    bool HasRemainingStages() const;
    bool IsRunCleared() const;
    bool IsRunFailed() const;

    int32 GetCurrentStageIndex() const;
    int32 GetMaxStageCount() const;
    int32 GetTotalSignal() const;
    int32 GetRequiredTotalSignal() const;

    /* Seed */
    int32 GetRunSeed() const;
    int32 GetCurrentStageSeed() const;

public:
    UPROPERTY(BlueprintAssignable)
    FOnRunCleared OnRunCleared;

    UPROPERTY(BlueprintAssignable)
    FOnRunFailed OnRunFailed;

    UPROPERTY(BlueprintAssignable)
    FOnStageIndexChanged OnStageIndexChanged;

private:
    void AdvanceStage();

private:
    /* Config */
    UPROPERTY()
    TObjectPtr<USignalRunConfig> RunConfig;

    /* Run 상태 */
    int32 RunSeed = 0;
    int32 CurrentStageIndex = 0;
    int32 TotalCollectedSignal = 0;
};
