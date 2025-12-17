// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SignalGameplayTags.generated.h"

USTRUCT()
struct FSignalGameplayTags
{
    GENERATED_BODY()

public:
    static const FSignalGameplayTags& Get();
    static void InitializeNativeTags();

    // Ability
    FGameplayTag Ability_Drone_LightToggle;
    FGameplayTag Ability_Drone_Scan;
    FGameplayTag Ability_Drone_Extract;
    FGameplayTag Ability_Drone_PassiveBattery;
    FGameplayTag Ability_Drone_Exit;

    // State
    FGameplayTag State_Drone_LightOn;
    FGameplayTag State_Drone_LowBattery;
    FGameplayTag State_Drone_Charging;
    FGameplayTag State_Drone_ScanHit;

    // Effect
    FGameplayTag Effect_Battery_Drain_Base;
    FGameplayTag Effect_Battery_Drain_Light;
    FGameplayTag Effect_Battery_Regen_Charge;

    // GCue
    FGameplayTag GCue_Drone_ScanPulse;

    // Input
    FGameplayTag Input_Drone_LightToggle;
    FGameplayTag Input_Drone_Scan;
    FGameplayTag Input_Drone_Extract;
    FGameplayTag Input_Drone_Exit;

    // SetByCaller
    FGameplayTag SetByCaller_BatteryDelta;
};
