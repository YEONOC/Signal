// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "DroneHUDWidget.generated.h"


class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
class UDroneCoreAttributeSet;

/**
 * 
 */
UCLASS()
class SIGNAL_API UDroneHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // ASC/AttributeSet 넘겨서 초기화
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void InitializeFromASC(UAbilitySystemComponent* InASC, const UDroneCoreAttributeSet* InAttrSet);

protected:
    virtual void NativeDestruct() override;

    // UMG에서 바인딩할 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> BatteryBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> BatteryText;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> ASC;

    UPROPERTY()
    TObjectPtr<const UDroneCoreAttributeSet> AttrSet;

    FDelegateHandle BatteryChangedHandle;
    FDelegateHandle BatteryMaxChangedHandle;

    void OnBatteryChanged(const FOnAttributeChangeData& Data);
    void OnBatteryMaxChanged(const FOnAttributeChangeData& Data);

    void RefreshBatteryUI();   // Battery/BatteryMax로 UI 갱신

};
