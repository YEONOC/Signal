
#include "SignalFacilityGenerator.h"
#include "Engine/World.h"

// Sets default values
ASignalFacilityGenerator::ASignalFacilityGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASignalFacilityGenerator::BeginPlay()
{
    Super::BeginPlay();

    // 랜덤 시드 초기화
    RandomStream.Initialize(Seed);

    // 그리드 초기화
    Grid.Empty();
    Grid.SetNum(GridWidth * GridHeight);

    for (int32 y = 0; y < GridHeight; ++y)
    {
        for (int32 x = 0; x < GridWidth; ++x)
        {
            FSignalRoomCell& Cell = Grid[y * GridWidth + x];
            Cell.X = x;
            Cell.Y = y;
            Cell.RoomType = ESignalRoomType::Empty;
        }
    }

    // 1) 논리적인 방 배치
    GenerateLayout();

    // 2) 실제 Room Actor 스폰
    SpawnRooms();
}

/**
 * 메인 경로 + 사이드 룸 배치
 */
void ASignalFacilityGenerator::GenerateLayout()
{
    if (GridWidth < 3 || GridHeight < 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("FacilityGenerator: Grid size too small"));
        return;
    }

    // 시작 위치: 왼쪽 중간 정도
    int32 x = 1;
    int32 y = GridHeight / 2;

    FSignalRoomCell* StartCell = GetCell(x, y);
    if (!StartCell)
    {
        return;
    }
    StartCell->RoomType = ESignalRoomType::Start;

    // 메인 경로 길이
    const int32 PathLength = RandomStream.RandRange(5, 8);

    for (int32 i = 0; i < PathLength; ++i)
    {
        // 오른쪽으로 전진하면서, 약간 위/아래로 랜덤
        int32 DirY = RandomStream.RandRange(-1, 1);
        y = FMath::Clamp(y + DirY, 1, GridHeight - 2);
        x = FMath::Clamp(x + 1, 1, GridWidth - 2);

        FSignalRoomCell* Cell = GetCell(x, y);
        if (Cell)
        {
            if (Cell->RoomType == ESignalRoomType::Empty)
            {
                Cell->RoomType = ESignalRoomType::Corridor;
            }
        }
    }

    // 마지막 방은 Objective로 변경
    if (FSignalRoomCell* LastCell = GetCell(x, y))
    {
        LastCell->RoomType = ESignalRoomType::Objective;
    }

    // 사이드 룸 추가 (Storage / PowerRoom)
    AddSideRooms();
}

/**
 * 메인 경로 옆으로 Storage / PowerRoom 추가
 */
void ASignalFacilityGenerator::AddSideRooms()
{
    for (FSignalRoomCell& Cell : Grid)
    {
        if (Cell.RoomType == ESignalRoomType::Corridor)
        {
            // 일정 확률로 사이드 방 생성
            if (RandomStream.FRand() < 0.6f)
            {
                const int32 SideDir = (RandomStream.FRand() < 0.5f) ? -1 : 1;
                const int32 nx = Cell.X;
                const int32 ny = Cell.Y + SideDir;

                FSignalRoomCell* SideCell = GetCell(nx, ny);
                if (SideCell && SideCell->RoomType == ESignalRoomType::Empty)
                {
                    const float r = RandomStream.FRand();
                    if (r < 0.7f)
                    {
                        SideCell->RoomType = ESignalRoomType::Storage;
                    }
                    else
                    {
                        SideCell->RoomType = ESignalRoomType::PowerRoom;
                    }
                }
            }
        }
    }
}

/**
 * RoomSet을 참고해서 실질적인 방 Actor 스폰
 */
void ASignalFacilityGenerator::SpawnRooms()
{
    if (!RoomSet)
    {
        UE_LOG(LogTemp, Warning, TEXT("FacilityGenerator: RoomSet is nullptr"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (const FSignalRoomCell& Cell : Grid)
    {
        if (Cell.RoomType == ESignalRoomType::Empty)
        {
            continue;
        }

        const FSignalRoomArchetype* Archetype = RoomSet->FindArchetype(Cell.RoomType);
        if (!Archetype || Archetype->RoomBlueprints.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Room Archetype or Blueprints for RoomType %d"),
                static_cast<int32>(Cell.RoomType));
            continue;
        }

        const int32 Index = RandomStream.RandRange(0, Archetype->RoomBlueprints.Num() - 1);
        TSubclassOf<AActor> RoomClass = Archetype->RoomBlueprints[Index];

        if (!*RoomClass)
        {
            continue;
        }

        const FVector SpawnLocation = GetActorLocation() + FVector(
            Cell.X * CellSize,
            Cell.Y * CellSize,
            0.f
        );
        const FRotator SpawnRotation = FRotator::ZeroRotator;

        World->SpawnActor<AActor>(RoomClass, SpawnLocation, SpawnRotation);
    }
}

/**
 * (X, Y) 셀 포인터 반환
 */
FSignalRoomCell* ASignalFacilityGenerator::GetCell(int32 X, int32 Y)
{
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        return nullptr;
    }

    return &Grid[Y * GridWidth + X];
}

const FSignalRoomCell* ASignalFacilityGenerator::GetCell(int32 X, int32 Y) const
{
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        return nullptr;
    }

    return &Grid[Y * GridWidth + X];
}
