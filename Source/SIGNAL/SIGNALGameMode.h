// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SIGNALGameMode.generated.h"


class USignalRunSubsystem;
class ASignalGameState;
class USignalStageConfig;

UCLASS()
class ASIGNALGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    /* Stage 흐름 */
    void StartStage();
    void HandleStageCleared();

    /* Run 결과 */
    void HandleRunCleared();
    void HandleRunFailed();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Stage")
    TArray<TObjectPtr<USignalStageConfig>> StageConfigs;

    UPROPERTY(EditDefaultsOnly, Category = "Run")
    TObjectPtr<class USignalRunConfig> RunConfig;

private:
    UPROPERTY()
    TObjectPtr<USignalRunSubsystem> RunSubsystem;
	
};



