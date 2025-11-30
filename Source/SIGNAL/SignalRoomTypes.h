
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
};
