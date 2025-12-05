// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignalRoomTypes.h"
#include "SignalSpawnPoint.generated.h"

USTRUCT()
struct FSignalItemSpawnPoint
{
    GENERATED_BODY()

    // 이 스폰 포인트가 속한 방 타입
    UPROPERTY()
    ESignalRoomType RoomType = ESignalRoomType::Empty;

    // 실제 월드 위치
    UPROPERTY()
    FVector WorldLocation = FVector::ZeroVector;


    // 나중에 필요하면 참조용
    UPROPERTY()
    TWeakObjectPtr<AActor> OwnerRoom;

    UPROPERTY()
    TWeakObjectPtr<USceneComponent> PointComponent;
};

USTRUCT()
struct FSignalEnemySpawnPoint
{
    GENERATED_BODY()

    UPROPERTY()
    ESignalRoomType RoomType = ESignalRoomType::Empty;

    UPROPERTY()
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY()
    TWeakObjectPtr<AActor> OwnerRoom;

    UPROPERTY()
    TWeakObjectPtr<USceneComponent> PointComponent;
};

