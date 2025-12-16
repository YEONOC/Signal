// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SignalGameState.generated.h"

class USignalStageConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignalAmountChanged, int32, NewSignal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageCleared);

UCLASS()
class SIGNAL_API ASignalGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ASignalGameState();

    // 신호 초기화 (게임 시작 시 호출)
    UFUNCTION(BlueprintCallable, Category = "Signal")
    void InitializeSignalFromStage();

    // 신호 추가 (아이템 추출 완료 시 호출)
    UFUNCTION(BlueprintCallable, Category = "Signal")
    void AddSignal(int32 Amount);

    // 현재 클리어 상태인지 체크
    UFUNCTION(BlueprintCallable, Category = "Signal")
    bool IsStageCleared() const;

protected:
    void HandleStageCleared();

public:
    // 현재 플레이 중인 스테이지 설정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
    TObjectPtr<USignalStageConfig> StageConfig;

    // 현재까지 모은 신호량
    UPROPERTY(BlueprintReadOnly, Category = "Signal")
    int32 CurrentSignal;

    // 신호량이 변경될 때 브로드캐스트 (HUD 연동용)
    UPROPERTY(BlueprintAssignable, Category = "Signal")
    FOnSignalAmountChanged OnSignalAmountChanged;

    // 클리어 시 브로드캐스트 (GameMode/HUD에서 받기)
    UPROPERTY(BlueprintAssignable, Category = "Signal")
    FOnStageCleared OnStageCleared;
};

