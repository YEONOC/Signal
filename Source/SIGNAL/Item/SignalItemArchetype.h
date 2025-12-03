// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignalRoomTypes.h"
#include "Engine/DataTable.h"
#include "SignalItemArchetype.generated.h"

USTRUCT(BlueprintType)
struct SIGNAL_API FSignalItemArchetype
{
    GENERATED_BODY()

public:
    // 아이템 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ItemId;

    // 실제 월드에 스폰되는 Actor 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> ItemClass;

    // 이 아이템에서 추출 가능한 신호량 범위
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    int32 SignalYieldMin = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    int32 SignalYieldMax = 15;

    // 추출하는 데 필요한 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    float ExtractTime = 2.0f;

    // 추출 시 발생하는 소음 (적 AI 반응 용도)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    float NoiseLevel = 1.0f;

    // 신호 중요도 UI/Outline 표시용 (0=일반, 3=희귀)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Signal")
    int32 SignalGrade = 0;

    // 이 아이템이 주로 등장하는 RoomType
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<ESignalRoomType> PreferredRooms;

    // 스폰 확률 가중치
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SpawnWeight = 10;
};
