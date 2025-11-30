// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SignalRoomTypes.h"
#include "SignalRoomSet.generated.h"

/**
 * 특정 RoomType에 대해 선택할 수 있는 방 Blueprint 목록
 */
USTRUCT(BlueprintType)
struct SIGNAL_API FSignalRoomArchetype
{
    GENERATED_BODY()

public:
    // 어떤 RoomType에 대한 설정인지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    ESignalRoomType RoomType = ESignalRoomType::Empty;

    // 이 RoomType을 생성할 때 랜덤으로 선택할 Room Blueprint들
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    TArray<TSubclassOf<AActor>> RoomBlueprints;
};

/**
 * 
 */
UCLASS(BlueprintType)
class SIGNAL_API USignalRoomSet : public UDataAsset
{
    GENERATED_BODY()

public:
    // 사용 가능한 방 Archetype 목록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rooms")
    TArray<FSignalRoomArchetype> RoomTypes;

public:
    // 주어진 RoomType에 해당하는 Archetype을 찾아 반환 (없으면 nullptr)
    const FSignalRoomArchetype* FindArchetype(ESignalRoomType RoomType) const;
};
