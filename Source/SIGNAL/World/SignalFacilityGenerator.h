#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalRoomTypes.h"
#include "World/SignalRoomSet.h"
#include "SignalFacilityGenerator.generated.h"

/**
 * 프로시저럴 시설(던전) 생성기 액터
 * - 그리드 기반으로 RoomCell들을 생성하고
 * - USignalRoomSet을 참고해서 실제 Room BP를 스폰
 */
UCLASS()
class SIGNAL_API ASignalFacilityGenerator : public AActor
{
    GENERATED_BODY()

public:
    ASignalFacilityGenerator();

protected:
    virtual void BeginPlay() override;

public:
    // 그리드 가로/세로 크기
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
    int32 GridWidth = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
    int32 GridHeight = 10;

    // 각 셀 사이의 실제 월드 거리 (방 간 간격)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
    float CellSize = 3000.f;

    // 사용 가능한 방 세트 (DataAsset)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
    TObjectPtr<USignalRoomSet> RoomSet;

    // 동일 Seed로 항상 동일한 맵 생성
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generator")
    int32 Seed = 12345;

private:
    // 그리드 전체 데이터
    UPROPERTY()
    TArray<FSignalRoomCell> Grid;

    // 내부에서 사용할 랜덤 스트림
    FRandomStream RandomStream;

private:
    // 레이아웃 생성: RoomType 배치
    void GenerateLayout();

    // 사이드 룸(파밍/충전용) 추가
    void AddSideRooms();

    // 실제 방 Actor 스폰
    void SpawnRooms();

    // (X, Y)에 해당하는 Cell 포인터 반환 (범위 밖이면 nullptr)
    FSignalRoomCell* GetCell(int32 X, int32 Y);
    const FSignalRoomCell* GetCell(int32 X, int32 Y) const;
};
