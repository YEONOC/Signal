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

void UDroneHUDWidget::SetExtractCandidate(const FString& TargetName, float Distance, bool bHasTarget)
{
    if (!ExtractTargetText || !ExtractDistanceText)
        return;

    if (!bHasTarget)
    {
        ExtractTargetText->SetText(FText::FromString(TEXT("Target: -")));
        ExtractDistanceText->SetText(FText::FromString(TEXT("-")));
        SetExtractProgress(0.f);
        SetExtractState(EExtractHUDState::NoTarget);
        ApplyExtractVisibility(false);
        return;
    }

    ApplyExtractVisibility(true);

    ExtractTargetText->SetText(FText::FromString(FString::Printf(TEXT("Target: %s"), *TargetName)));
    ExtractDistanceText->SetText(FText::FromString(FString::Printf(TEXT("%.1fm"), Distance)));

    // 후보 상태로 유지 (추출 중이면 Extracting이 우선)
    if (CurrentExtractState != EExtractHUDState::Extracting)
    {
        SetExtractState(EExtractHUDState::Candidate);
        SetExtractProgress(0.f);
    }
}

void UDroneHUDWidget::SetExtractProgress(float Alpha)
{
    if (!ExtractMID)
        return;

    ExtractMID->SetScalarParameterValue(TEXT("Progress"), Alpha);
}

void UDroneHUDWidget::SetExtractState(EExtractHUDState NewState, const FString& OptionalMessage)
{
    CurrentExtractState = NewState;

    if (Img_ExtractRing)
    {
        const bool bShow = (NewState == EExtractHUDState::Extracting);
        Img_ExtractRing->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
    }

    if (!ExtractMID)
        return;

    // 상태에 따라 패널 표시 정책
    if (NewState == EExtractHUDState::Hidden)
    {
        ApplyExtractVisibility(false);
        return;
    }

    ApplyExtractVisibility(true);
    RefreshExtractStateText(NewState, OptionalMessage);

    // Completed/Cancelled는 Progress를 0으로 리셋하거나 유지 정책 선택 가능
    if (NewState == EExtractHUDState::Cancelled || NewState == EExtractHUDState::Completed)
    {
        if (NewState == EExtractHUDState::Cancelled)
        {
            SetExtractProgress(0.f);
        }
    }
}

void UDroneHUDWidget::NativeConstruct()
{
    if (Img_ExtractRing)
    {
        Img_ExtractRing->SetVisibility(ESlateVisibility::Collapsed);
        ExtractMID = Img_ExtractRing->GetDynamicMaterial();
        SetExtractProgress(0.f);
    }

    Super::NativeConstruct();
}

void UDroneHUDWidget::NativeDestruct()
{
    if (ASC)
    {
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
    }

    Super::NativeDestruct();
}

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
    // UE_LOG(LogTemp, Warning, TEXT("[HUD] Battery %.1f / %.1f"), Battery, BatteryMax);
}

void UDroneHUDWidget::ApplyExtractVisibility(bool bVisible)
{
    if (ExtractPanel)
    {
        ExtractPanel->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UDroneHUDWidget::RefreshExtractStateText(EExtractHUDState State, const FString& OptionalMessage)
{
    if (!ExtractStateText)
        return;

    FString Msg = OptionalMessage;

    if (Msg.IsEmpty())
    {
        switch (State)
        {
        case EExtractHUDState::Hidden:     Msg = TEXT(""); break;
        case EExtractHUDState::NoTarget:   Msg = TEXT("NO TARGET"); break;
        case EExtractHUDState::Candidate:  Msg = TEXT("READY"); break;
        case EExtractHUDState::Extracting: Msg = TEXT("EXTRACTING..."); break;
        case EExtractHUDState::Cancelled:  Msg = TEXT("CANCELLED"); break;
        case EExtractHUDState::Completed:  Msg = TEXT("COMPLETE"); break;
        case EExtractHUDState::OutOfRange: Msg = TEXT("OUT OF RANGE"); break;
        default: Msg = TEXT(""); break;
        }
    }

    ExtractStateText->SetText(FText::FromString(Msg));
}
