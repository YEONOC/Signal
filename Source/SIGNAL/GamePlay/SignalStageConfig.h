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

    UPROPERTY(EditAnywhere, Category = "Signal Supply")
    int32 TargetSignalSupply = 100;

    UPROPERTY(EditAnywhere, Category = "Signal Supply")
    float SupplyVariance = 0.3f;

    /*
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stage")
    TObjectPtr<USignalEnemySet> EnemySet;*/
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stage")
    TObjectPtr<USignalItemSet> ItemSet;
};
