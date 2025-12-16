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
class UPanelWidget;
class UImage;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EExtractHUDState : uint8
{
    Hidden      UMETA(DisplayName = "Hidden"),
    NoTarget    UMETA(DisplayName = "NoTarget"),
    Candidate   UMETA(DisplayName = "Candidate"),
    Extracting  UMETA(DisplayName = "Extracting"),
    Cancelled   UMETA(DisplayName = "Cancelled"),
    Completed   UMETA(DisplayName = "Completed"),
    OutOfRange  UMETA(DisplayName = "OutOfRange")
};

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

    UFUNCTION(BlueprintCallable, Category = "HUD|Extract")
    void SetExtractCandidate(const FString& TargetName, float Distance, bool bHasTarget);

    UFUNCTION(BlueprintCallable, Category = "HUD|Extract")
    void SetExtractProgress(float Alpha); // 0~1

    UFUNCTION(BlueprintCallable, Category = "HUD|Extract")
    void SetExtractState(EExtractHUDState NewState, const FString& OptionalMessage = TEXT(""));

protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

protected:

    // Battery UI
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> BatteryBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> BatteryText;

    // Extract UI
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> ExtractPanel;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> ExtractTargetText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> ExtractDistanceText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> ExtractStateText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Img_ExtractRing;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> ExtractMID;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> ASC;

    // Battery
    UPROPERTY()
    TObjectPtr<const UDroneCoreAttributeSet> AttrSet;

    FDelegateHandle BatteryChangedHandle;
    FDelegateHandle BatteryMaxChangedHandle;

    void OnBatteryChanged(const FOnAttributeChangeData& Data);
    void OnBatteryMaxChanged(const FOnAttributeChangeData& Data);

    void RefreshBatteryUI();   // Battery/BatteryMax로 UI 갱신

    // Extract
    EExtractHUDState CurrentExtractState = EExtractHUDState::Hidden;

    void ApplyExtractVisibility(bool bVisible);
    void RefreshExtractStateText(EExtractHUDState State, const FString& OptionalMessage);
};
