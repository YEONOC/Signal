#include "SignalGameplayTags.h"
#include "GameplayTagsManager.h"

FSignalGameplayTags GameplayTagsInstance;
static bool bTagsInitialized = false;

const FSignalGameplayTags& FSignalGameplayTags::Get()
{
    if (!bTagsInitialized)
    {
        InitializeNativeTags();
        bTagsInitialized = true;
    }
    return GameplayTagsInstance;
}

void FSignalGameplayTags::InitializeNativeTags()
{
    // 이미 초기화 되어 있으면 다시 안 함 (중복 방지)
    if (bTagsInitialized)
    {
        return;
    }

    // 헬퍼 람다: 태그 생성 + 저장
    auto AddTag = [](FGameplayTag& OutTag, const ANSICHAR* TagName, const TCHAR* DevComment)
        {
            OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName(TagName),
                DevComment
            );
        };

    // Ability
    AddTag(GameplayTagsInstance.Ability_Drone_LightToggle, "Ability.Drone.LightToggle", TEXT("Toggle drone light on/off"));
    AddTag(GameplayTagsInstance.Ability_Drone_Scan, "Ability.Drone.Scan", TEXT("Drone scan ability"));
    AddTag(GameplayTagsInstance.Ability_Drone_Dash, "Ability.Drone.Dash", TEXT("Drone dash ability"));
    AddTag(GameplayTagsInstance.Ability_Drone_Interact, "Ability.Drone.Interact", TEXT("Drone interact ability"));
    AddTag(GameplayTagsInstance.Ability_Drone_PassiveBattery, "Ability.Drone.PassiveBattery", TEXT("Passive battery system ability"));

    // State
    AddTag(GameplayTagsInstance.State_Drone_LightOn, "State.Drone.LightOn", TEXT("Drone light is currently on"));
    AddTag(GameplayTagsInstance.State_Drone_LowBattery, "State.Drone.LowBattery", TEXT("Drone battery is low"));
    AddTag(GameplayTagsInstance.State_Drone_Charging, "State.Drone.Charging", TEXT("Drone is charging"));
    AddTag(GameplayTagsInstance.State_Drone_Dashing, "State.Drone.Dashing", TEXT("Drone is dashing"));
    AddTag(GameplayTagsInstance.State_Drone_ScanHit, "State.Drone.ScanHit", TEXT("Drone is ScanHit"));

    // Effect
    AddTag(GameplayTagsInstance.Effect_Battery_Drain_Base, "Effect.Battery.Drain.Base", TEXT("Base battery drain"));
    AddTag(GameplayTagsInstance.Effect_Battery_Drain_Light, "Effect.Battery.Drain.Light", TEXT("Additional drain while light is on"));
    AddTag(GameplayTagsInstance.Effect_Battery_Regen_Charge, "Effect.Battery.Regen.Charge", TEXT("Battery regen while charging"));

    // GameplayCue
    AddTag(GameplayTagsInstance.GCue_Drone_ScanPulse, "GameplayCue.Drone.ScanPulse", TEXT("Scan pulse visual/audio cue"));

    // Input
    AddTag(GameplayTagsInstance.Input_Drone_LightToggle, "Input.Drone.LightToggle", TEXT("Input for toggling drone light"));
    AddTag(GameplayTagsInstance.Input_Drone_Scan, "Input.Drone.Scan", TEXT("Input for drone scan pulse"));

    // SetByCaller
    AddTag(GameplayTagsInstance.SetByCaller_BatteryDelta, "SetByCaller.BatteryDelta", TEXT("SetByCaller for changing battery value"));
}
