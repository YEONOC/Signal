
#pragma once

#include "CoreMinimal.h"
#include "SignalRoomTypes.generated.h"

UENUM(BlueprintType)
enum class ESignalRoomType : uint8
{
    Empty       UMETA(DisplayName = "Empty"),
    Start       UMETA(DisplayName = "Start"),
    Corridor    UMETA(DisplayName = "Corridor"),
    Storage     UMETA(DisplayName = "Storage"),
    PowerRoom   UMETA(DisplayName = "PowerRoom"),
    Lab         UMETA(DisplayName = "Lab"),
    Objective   UMETA(DisplayName = "Objective")
};

/** 문의 위치가 고정인 경우 해당 문의 방향 */
UENUM(BlueprintType)
enum class ESignalDoorDirection : uint8
{
    None,
    North,
    East,
    South,
    West
};

/** 방의 각 방향에 문이 열려 있는지 여부 */
USTRUCT(BlueprintType)
struct SIGNAL_API FSignalRoomDoors
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bNorth = false;   // +Y

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEast = false;    // +X

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSouth = false;   // -Y

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWest = false;    // -X

    // 이 방이 "한 방향으로만" 열려 있다고 가정하고, 그 방향을 반환
    // Storage, PowerRoom 같은 측면방(사이드룸, 단일 입구)
    ESignalDoorDirection GetSingleOpenDirection() const
    {
        if (bNorth && !bEast && !bSouth && !bWest)
        {
            return ESignalDoorDirection::North;
        }
        if (!bNorth && bEast && !bSouth && !bWest)
        {
            return ESignalDoorDirection::East;
        }
        if (!bNorth && !bEast && bSouth && !bWest)
        {
            return ESignalDoorDirection::South;
        }
        if (!bNorth && !bEast && !bSouth && bWest)
        {
            return ESignalDoorDirection::West;
        }

        // 그 외(여러 방향 열려 있거나 전부 닫혀 있음)
        return ESignalDoorDirection::None;
    }
};

USTRUCT(BlueprintType)
struct FSignalRoomCell
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESignalRoomType RoomType = ESignalRoomType::Empty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 X = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Y = 0;

    // 이 셀에서 어느 방향으로 문이 열려 있는지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSignalRoomDoors Doors;

    // 실제 스폰된 Room Actor (디버그 & 스폰 포인트 수집용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TWeakObjectPtr<AActor> SpawnedRoomActor;
};
