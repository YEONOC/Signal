// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"

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

        BatteryText->SetText(
            FText::FromString(FString::Printf(TEXT("%d / %d"), BatteryInt, BatteryMaxInt)));
    }

    // 디버그용
    // UE_LOG(LogTemp, Warning, TEXT("[HUD] Battery %.1f / %.1f"), Battery, BatteryMax);
}