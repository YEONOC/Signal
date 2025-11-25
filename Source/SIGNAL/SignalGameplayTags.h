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
    FGameplayTag Ability_Drone_ScanPulse;
    FGameplayTag Ability_Drone_Dash;
    FGameplayTag Ability_Drone_Interact;
    FGameplayTag Ability_Drone_PassiveBattery;

    // State
    FGameplayTag State_Drone_LightOn;
    FGameplayTag State_Drone_LowBattery;
    FGameplayTag State_Drone_Charging;
    FGameplayTag State_Drone_Dashing;
    FGameplayTag State_Drone_Scanning;

    // Effect
    FGameplayTag Effect_Battery_Drain_Base;
    FGameplayTag Effect_Battery_Drain_Light;
    FGameplayTag Effect_Battery_Regen_Charge;

    // GCue
    FGameplayTag GCue_Drone_ScanPulse;

    // Input
    FGameplayTag Input_Drone_LightToggle;

    // SetByCaller
    FGameplayTag SetByCaller_BatteryDelta;
};
