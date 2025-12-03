// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SIGNALGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ASIGNALGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    /** Constructor */
    ASIGNALGameMode();

public:

    // 이 스테이지에서 사용할 설정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
    TObjectPtr<class USignalStageConfig> DefaultStageConfig;

    virtual void BeginPlay() override;

	
};



