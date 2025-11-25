#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SignalGameplayAbility.generated.h"

UCLASS()
class SIGNAL_API USignalGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    USignalGameplayAbility();

    /** 이 Ability의 입력 태그 (예: Ability.Drone.LightToggle) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    FGameplayTag AbilityInputTag;
};