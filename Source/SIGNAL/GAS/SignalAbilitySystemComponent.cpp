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

void USignalAbilitySystemComponent::CancelAbilitiesByInputTag(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
    {
        USignalGameplayAbility* SignalAbility = Cast<USignalGameplayAbility>(Spec.Ability);
        if (!SignalAbility)
        {
            continue;
        }

        // 이 어빌리티가 어떤 InputTag에 묶여 있는지 검사
        if (SignalAbility->AbilityInputTag == InputTag)
        {
            // 이 스펙으로 활성화된 인스턴스들 취소
            CancelAbilityHandle(Spec.Handle);
        }
    }
}
