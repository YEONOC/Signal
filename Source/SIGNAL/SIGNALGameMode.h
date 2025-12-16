// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SIGNALGameMode.generated.h"


class USignalRunSubsystem;
class USignalStageConfig;
class USignalRunConfig;
class ASignalFacilityGenerator;

UCLASS()
class ASIGNALGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetFacilityGenerator(ASignalFacilityGenerator* InGenerator)
    {
        FacilityGenerator = InGenerator;
    }

protected:
    virtual void BeginPlay() override;

    /* Stage 흐름 */
    void StartStage();
    UFUNCTION()
    void HandleStageCleared();
    UFUNCTION()
    void HandleStageFailed();

    /* Run 결과 */
    UFUNCTION()
    void HandleRunCleared();
    UFUNCTION()
    void HandleRunFailed();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Stage")
    TArray<TObjectPtr<USignalStageConfig>> StageConfigs;

    UPROPERTY(EditDefaultsOnly, Category = "Run")
    TObjectPtr<USignalRunConfig> RunConfig;

private:
    UPROPERTY()
    TObjectPtr<USignalRunSubsystem> RunSubsystem;

    UPROPERTY()
    TObjectPtr<ASignalFacilityGenerator> FacilityGenerator;
	
};



