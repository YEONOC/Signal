// Fill out your copyright notice in the Description page of Project Settings.

#include "SignalInputConfig.h"
#include "InputAction.h"

const UInputAction* USignalInputConfig::FindInputActionByTag(const FGameplayTag& InTag) const
{
    for (const FSignalInputAction& Entry : AbilityInputActions)
    {
        if (Entry.InputTag == InTag)
        {
            return Entry.InputAction;
        }
    }
    return nullptr;
}
