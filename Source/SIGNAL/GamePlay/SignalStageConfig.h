// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SignalStageConfig.generated.h"

class USignalItemSet;

UCLASS(BlueprintType)
class SIGNAL_API USignalStageConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // 이 Stage의 이름/ID (UI 표기용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
    FName StageId;

    // 적 난이도 (후에 EnemySpawn에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
    int32 ThreatLevel = 1;

    // 맵 크기나 복잡도 (원하면 FacilityGenerator 튜닝에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
    int32 MapSize = 1;

    // 💡 스테이지 클리어에 필요한 신호량
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    int32 RequiredSignal = 100;

    // 이 이상 모으면 보너스 보상 기준
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    int32 BonusSignalTarget = 150;

     /*나중에 EnemySet / ItemSet 같은 것도 여기 연결 가능
     UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stage")
     TObjectPtr<USignalEnemySet> EnemySet;*/
    
     UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stage")
     TObjectPtr<USignalItemSet> ItemSet;
};
