// Copyright Epic Games, Inc. All Rights Reserved.

#include "SIGNAL.h"
#include "Modules/ModuleManager.h"
#include "SignalGameplayTags.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FSignalModule, SIGNAL, "SIGNAL" );

void FSignalModule::StartupModule()
{
    FDefaultGameModuleImpl::StartupModule();

    // Initialize native gameplay tags
    FSignalGameplayTags::InitializeNativeTags();
}

DEFINE_LOG_CATEGORY(LogSIGNAL)