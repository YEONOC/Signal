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

    /** 이 Ability의 입력 태그 (Input_Drone_XXX) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Signal | Input")
    FGameplayTag AbilityInputTag;
};