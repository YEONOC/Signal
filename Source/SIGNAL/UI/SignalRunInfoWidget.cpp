// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SignalRunInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void USignalRunInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USignalRunInfoWidget::UpdateRunInfo(
	int32 CurrentStageIndex,
	int32 MaxStageCount,
	int32 CurrentStageSignal,
	int32 TotalRunSignal,
	int32 RequiredTotalSignal
)
{
	// Stage 정보 업데이트
	if (StageInfoText)
	{
		StageInfoText->SetText(FormatStageInfo(CurrentStageIndex, MaxStageCount));
	}

	// 현재 스테이지 Signal 업데이트
	if (CurrentStageSignalText)
	{
		CurrentStageSignalText->SetText(FText::Format(
			NSLOCTEXT("SignalRunInfo", "CurrentStageSignal", "현재 스테이지: {0} Signal"),
			CurrentStageSignal
		));
	}

	// 전체 Run Signal 업데이트
	if (TotalRunSignalText)
	{
		TotalRunSignalText->SetText(FText::Format(
			NSLOCTEXT("SignalRunInfo", "TotalRunSignal", "누적 Signal: {0}"),
			TotalRunSignal
		));
	}

	// 목표 Signal 업데이트
	if (RequiredSignalText)
	{
		RequiredSignalText->SetText(FText::Format(
			NSLOCTEXT("SignalRunInfo", "RequiredSignal", "목표 Signal: {0}"),
			RequiredTotalSignal
		));
	}
}

FText USignalRunInfoWidget::FormatStageInfo(int32 Current, int32 Max) const
{
	return FText::Format(
		NSLOCTEXT("SignalRunInfo", "StageInfo", "Stage {0} / {1}"),
		Current,
		Max
	);
}

FText USignalRunInfoWidget::FormatSignalInfo(int32 Current, int32 Max) const
{
	return FText::Format(
		NSLOCTEXT("SignalRunInfo", "SignalInfo", "{0} / {1}"),
		Current,
		Max
	);
}

