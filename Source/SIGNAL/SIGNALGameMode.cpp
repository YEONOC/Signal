// Copyright Epic Games, Inc. All Rights Reserved.

#include "SIGNALGameMode.h"
#include "SignalGameState.h"
#include "Kismet/GameplayStatics.h"

ASIGNALGameMode::ASIGNALGameMode()
{
	// stub
}

void ASIGNALGameMode::BeginPlay()
{
    Super::BeginPlay();

    ASignalGameState* GS = GetGameState<ASignalGameState>();
    if (GS)
    {
        GS->StageConfig = DefaultStageConfig;
        GS->InitializeSignalFromStage();
    }
}


