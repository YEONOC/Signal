// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDroneHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Interact Image 기반이면 MID 생성
    if (InteractProgressImage)
    {
        UMaterialInterface* BaseMat = InteractProgressImage->GetBrush().GetResourceObject()
            ? Cast<UMaterialInterface>(InteractProgressImage->GetBrush().GetResourceObject())
            : nullptr;

        if (BaseMat)
        {
            InteractMID = UMaterialInstanceDynamic::Create(BaseMat, this);
            InteractProgressImage->SetBrushFromMaterial(InteractMID);
        }
    }

    // 초기 UI 상태
    InteractState = EInteractHUDState::None;
    InteractProgress01 = 0.f;
    RefreshInteractUI();
}

void UDroneHUDWidget::NativeDestruct()
{
    if (ASC)
    {
        if (BatteryChangedHandle.IsValid())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(UDroneCoreAttributeSet::GetBatteryAttribute())
                .Remove(BatteryChangedHandle);
        }
        if (BatteryMaxChangedHandle.IsValid())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(UDroneCoreAttributeSet::GetBatteryMaxAttribute())
                .Remove(BatteryMaxChangedHandle);
        }
    }

    ASC = nullptr;
    AttrSet = nullptr;

    Super::NativeDestruct();
}

void UDroneHUDWidget::InitializeFromASC(UAbilitySystemComponent* InASC, const UDroneCoreAttributeSet* InAttrSet)
{
    ASC = InASC;
    AttrSet = InAttrSet;

    if (!ASC || !AttrSet)
    {
        return;
    }

    // 기존 델리게이트 제거 (재초기화 대비)
    if (BatteryChangedHandle.IsValid())
    {
        ASC->GetGameplayAttributeValueChangeDelegate(AttrSet->GetBatteryAttribute())
            .Remove(BatteryChangedHandle);
    }
    if (BatteryMaxChangedHandle.IsValid())
    {
        ASC->GetGameplayAttributeValueChangeDelegate(AttrSet->GetBatteryMaxAttribute())
            .Remove(BatteryMaxChangedHandle);
    }

    // 델리게이트 등록
    BatteryChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(AttrSet->GetBatteryAttribute())
        .AddUObject(this, &UDroneHUDWidget::OnBatteryChanged);

    BatteryMaxChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(AttrSet->GetBatteryMaxAttribute())
        .AddUObject(this, &UDroneHUDWidget::OnBatteryMaxChanged);

    // 처음 한 번 UI 갱신
    RefreshBatteryUI();
}

// ========================= Battery =========================

void UDroneHUDWidget::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
    RefreshBatteryUI();
}

void UDroneHUDWidget::OnBatteryMaxChanged(const FOnAttributeChangeData& Data)
{
    RefreshBatteryUI();
}

void UDroneHUDWidget::RefreshBatteryUI()
{
    if (!AttrSet) return;

    const float Battery = AttrSet->GetBattery();
    const float BatteryMax = AttrSet->GetBatteryMax();
    const float Percent = (BatteryMax > 0.f) ? Battery / BatteryMax : 0.f;

    if (BatteryBar)
    {
        BatteryBar->SetPercent(Percent);
    }

    if (BatteryText)
    {
        const int32 BatteryInt = FMath::RoundToInt(Battery);
        const int32 BatteryMaxInt = FMath::RoundToInt(BatteryMax);

        BatteryText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), BatteryInt, BatteryMaxInt)));
    }

    // 디버그용
    UE_LOG(LogTemp, Warning, TEXT("[HUD] Battery %.1f / %.1f"), Battery, BatteryMax);
}

// ========================= Interact (Extract / Exit) =========================

void UDroneHUDWidget::SetInteractTarget(FName TargetName)
{
    if (CachedTargetName == TargetName) 
        return;

    CachedTargetName = TargetName;
}

void UDroneHUDWidget::SetSignalAmount(int32 MinSignal, int32 MaxSignal)
{
    if (CachedSignalMin == MinSignal && CachedSignalMax == MaxSignal)
        return;

    CachedSignalMin = MinSignal;
    CachedSignalMax = MaxSignal;
}

void UDroneHUDWidget::SetInteractState(EInteractHUDState NewState)
{
    if (InteractState == NewState)
        return;

    InteractState = NewState;

    // Cancel/Completed/None이면 진행률 리셋하는 정책
    if (InteractState == EInteractHUDState::None ||
        InteractState == EInteractHUDState::Cancelled ||
        InteractState == EInteractHUDState::NoTarget)
    {
        InteractProgress01 = 0.f;
    }

    RefreshInteractUI();
}

void UDroneHUDWidget::SetInteractProgress(float Normalized01)
{
    InteractProgress01 = FMath::Clamp(Normalized01, 0.f, 1.f);

    // ProgressBar 기반
    if (InteractProgressBar)
    {
        InteractProgressBar->SetPercent(InteractProgress01);
    }

    // Image + MID 기반
    if (InteractMID)
    {
        InteractMID->SetScalarParameterValue(InteractProgressParamName, InteractProgress01);
    }
}

void UDroneHUDWidget::RefreshInteractUI()
{
    const bool bShow = ShouldShowInteractUI(InteractState);

    if (InteractStateText)
    {
        InteractStateText->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
        InteractStateText->SetText(FText::FromString(GetStateString(InteractState)));
    }

    if (InteractProgressBar)
    {
        InteractProgressBar->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
        // 상태 바뀐 경우 Percent도 맞춰줌
        InteractProgressBar->SetPercent(InteractProgress01);
    }

    if (InteractProgressImage)
    {
        InteractProgressImage->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

        if (InteractMID)
        {
            InteractMID->SetScalarParameterValue(InteractProgressParamName, InteractProgress01);
        }
    }

    // ===== Extract 전용 표시(타겟/예상 Signal) =====
    const bool bShowExtractInfo = (InteractState == EInteractHUDState::Extracting) && bShow;

    if (InteractTargetText)
    {
        InteractTargetText->SetVisibility(bShowExtractInfo ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
        if (bShowExtractInfo)
        {
            InteractTargetText->SetText(FText::FromName(CachedTargetName));
        }
    }

    if (SignalAmountText)
    {
        SignalAmountText->SetVisibility(bShowExtractInfo ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
        if (bShowExtractInfo)
        {
            SignalAmountText->SetText(
                FText::Format(
                    NSLOCTEXT("HUD", "ExpectedSignalFmt", "Expected Signal : {0} ~ {1}"),
                    FText::AsNumber(CachedSignalMin),
                    FText::AsNumber(CachedSignalMax)
                )
            );
        }
    }
}

bool UDroneHUDWidget::ShouldShowInteractUI(EInteractHUDState State) const
{
    switch (State)
    {
    case EInteractHUDState::Extracting:
    case EInteractHUDState::Exiting:
        return true;

        // Completed/Cancelled도 잠깐 보여주고 싶으면 true로 바꿀 수 있음
    case EInteractHUDState::Completed:
    case EInteractHUDState::Cancelled:
    case EInteractHUDState::NoTarget:
    case EInteractHUDState::None:
    default:
        return false;
    }
}

FString UDroneHUDWidget::GetStateString(EInteractHUDState State) const
{
    switch (State)
    {
    case EInteractHUDState::Extracting: return TEXT("EXTRACTING");
    case EInteractHUDState::Exiting:    return TEXT("EXITING");
    case EInteractHUDState::Completed:  return TEXT("COMPLETED");
    case EInteractHUDState::Cancelled:  return TEXT("CANCELLED");
    case EInteractHUDState::NoTarget:   return TEXT("NO TARGET");
    case EInteractHUDState::None:
    default:                            return TEXT("");
    }
}

