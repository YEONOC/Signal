#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../SignalRoomTypes.h"
#include "SignalRoomBase.generated.h"

/**
 * 모든 방 Blueprint가 상속받을 베이스 클래스
 * - 문 설정을 받아서 Blueprint 쪽에서 메쉬 Visibility를 조절
 */
UCLASS()
class SIGNAL_API ASignalRoomBase : public AActor
{
    GENERATED_BODY()

public:
    ASignalRoomBase();

    virtual void BeginPlay() override;

    // 방의 문 상태를 적용해달라는 요청
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Room")
    void ApplyDoorConfig(const FSignalRoomDoors& Doors);
    virtual void ApplyDoorConfig_Implementation(const FSignalRoomDoors& Doors);

public:
    // 이 Room이 어떤 RoomType인지 (Spawn 이후 Cell에서 세팅해줘도 됨)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
    ESignalRoomType RoomType = ESignalRoomType::Empty;

    // 아이템 스폰 포인트들 (BP에서 Arrow/SceneComponent들을 등록)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawns")
    TArray<USceneComponent*> ItemSpawnPoints;

    // 적 스폰 포인트들
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawns")
    TArray<USceneComponent*> EnemySpawnPoints;
};
