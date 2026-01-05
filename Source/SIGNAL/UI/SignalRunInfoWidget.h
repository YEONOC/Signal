// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignalRunInfoWidget.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * Run 정보를 표시하는 UI 위젯
 * - 현재 Stage 인덱스
 * - 전체 Stage 수
 * - 현재 스테이지 Signal
 * - 전체 Run Signal
 * - 목표 Signal
 */
UCLASS()
class SIGNAL_API USignalRunInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Run 정보 업데이트
	UFUNCTION(BlueprintCallable, Category = "Run Info")
	void UpdateRunInfo(
		int32 CurrentStageIndex,
		int32 MaxStageCount,
		int32 CurrentStageSignal,
		int32 TotalRunSignal,
		int32 RequiredTotalSignal
	);

protected:
	virtual void NativeConstruct() override;

protected:
	// Stage 정보
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StageInfoText;

	// 현재 스테이지 Signal
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentStageSignalText;

	// 전체 Run Signal
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalRunSignalText;

	// 목표 Signal
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RequiredSignalText;

private:
	// 텍스트 포맷팅 헬퍼 함수
	FText FormatStageInfo(int32 Current, int32 Max) const;
	FText FormatSignalInfo(int32 Current, int32 Max) const;
};

