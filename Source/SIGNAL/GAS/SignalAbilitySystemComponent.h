// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SignalAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SIGNAL_API USignalAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
    // 입력 태그로 Ability 실행
    bool TryActivateAbilitiesByInputTag(const FGameplayTag& InputTag);
};
