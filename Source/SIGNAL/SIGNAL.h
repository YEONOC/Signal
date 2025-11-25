// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSignalModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override;
};

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogSIGNAL, Log, All);