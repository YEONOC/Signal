// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SignalAbilitySystemComponent.h"
#include "GAS/Abilities/SignalGameplayAbility.h"
#include "SignalGameplayTags.h"

bool USignalAbilitySystemComponent::TryActivateAbilitiesByInputTag(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return false;
    }

    bool bActivatedAny = false;

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        USignalGameplayAbility* SignalAbility = Cast<USignalGameplayAbility>(Spec.Ability);
        if (!SignalAbility)
        {
            continue;
        }

        if (SignalAbility->AbilityInputTag == InputTag)
        {
            // 이 스펙의 Ability 실행 시도
            if (TryActivateAbility(Spec.Handle))
            {
                bActivatedAny = true;
            }
        }
    }

    return bActivatedAny;
}
