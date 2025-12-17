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
enum class EInteractHUDState : uint8
{
    None        UMETA(DisplayName = "None"),
    Extracting  UMETA(DisplayName = "Extracting"),
    Exiting     UMETA(DisplayName = "Exiting"),
    Completed   UMETA(DisplayName = "Completed"),
    Cancelled   UMETA(DisplayName = "Cancelled"),
    NoTarget    UMETA(DisplayName = "NoTarget"),
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

    // ===== Interact UI (Extract / Exit 공용) =====
    UFUNCTION(BlueprintCallable, Category = "HUD|Interact")
    void SetInteractState(EInteractHUDState NewState);

    UFUNCTION(BlueprintCallable, Category = "HUD|Interact")
    void SetInteractProgress(float Normalized01);

    UFUNCTION(BlueprintPure, Category = "HUD|Interact")
    EInteractHUDState GetInteractState() const { return InteractState; }

protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

protected:

    // Battery UI
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> BatteryBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> BatteryText;

    // Interact UI
    // 원형 ProgressBar를 쓰는 경우
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UProgressBar> InteractProgressBar = nullptr;

    // Image + Dynamic Material을 쓰는 경우
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> InteractProgressImage = nullptr;

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
    void RefreshInteractUI();

    UPROPERTY()
    EInteractHUDState InteractState = EInteractHUDState::None;

    UPROPERTY()
    float InteractProgress01 = 0.f;

    // Dynamic material 캐시 (Image 기반일 때)
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> InteractMID = nullptr;

    // 머티리얼 파라미터 이름(너가 만든 머티리얼에 맞춰 수정)
    UPROPERTY(EditDefaultsOnly, Category = "HUD|Interact")
    FName InteractProgressParamName = TEXT("Progress");

    // 상태별 표시 문자열(간단 버전)
    FString GetStateString(EInteractHUDState State) const;

    // UI 표시/숨김 규칙
    bool ShouldShowInteractUI(EInteractHUDState State) const;

};
