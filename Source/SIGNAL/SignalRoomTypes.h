
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
};
