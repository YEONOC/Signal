// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "Net/UnrealNetwork.h"

UDroneCoreAttributeSet::UDroneCoreAttributeSet()
{
    // 초기 값은 GameplayEffect로 세팅하는 걸 추천하지만,
    // 기본값 정도는 여기서도 넣어둘 수 있음.
}

void UDroneCoreAttributeSet::OnRep_Battery(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDroneCoreAttributeSet, Battery, OldValue);
}

void UDroneCoreAttributeSet::OnRep_BatteryMax(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDroneCoreAttributeSet, BatteryMax, OldValue);
}

void UDroneCoreAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UDroneCoreAttributeSet, Battery, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDroneCoreAttributeSet, BatteryMax, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDroneCoreAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDroneCoreAttributeSet, ScanRange, COND_None, REPNOTIFY_Always);
}