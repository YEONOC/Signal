// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DroneCoreAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class SIGNAL_API UDroneCoreAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UDroneCoreAttributeSet();

    // 배터리
    UPROPERTY(BlueprintReadOnly, Category = "Drone|Attributes", meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Battery;
    ATTRIBUTE_ACCESSORS(UDroneCoreAttributeSet, Battery);

    UPROPERTY(BlueprintReadOnly, Category = "Drone|Attributes", meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData BatteryMax;
    ATTRIBUTE_ACCESSORS(UDroneCoreAttributeSet, BatteryMax);

    // 이동 속도
    UPROPERTY(BlueprintReadOnly, Category = "Drone|Attributes", meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UDroneCoreAttributeSet, MoveSpeed);

    // 스캔 범위
    UPROPERTY(BlueprintReadOnly, Category = "Drone|Attributes", meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData ScanRange;
    ATTRIBUTE_ACCESSORS(UDroneCoreAttributeSet, ScanRange);

protected:
    UFUNCTION()
    void OnRep_Battery(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_BatteryMax(const FGameplayAttributeData& OldValue);
	
};
