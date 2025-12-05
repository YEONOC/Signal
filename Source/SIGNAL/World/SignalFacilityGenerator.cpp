
#include "SignalFacilityGenerator.h"
// Room 생성
#include "Engine/World.h"
#include "SignalRoomBase.h"
// 아이템 및 적 생성
#include "SignalSpawnPoint.h"
#include "SignalGameState.h"
#include "GamePlay/SignalStageConfig.h"
#include "Item/SignalItemActor.h"
#include "Item/SignalItemArchetype.h"
#include "Item/SignalItemSet.h"

// Sets default values
ASignalFacilityGenerator::ASignalFacilityGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASignalFacilityGenerator::DebugLogGridLayout() const
{
    UE_LOG(LogTemp, Warning, TEXT("==== Facility Layout (Seed=%d) ===="), Seed);
    UE_LOG(LogTemp, Warning, TEXT("GridWidth=%d, GridHeight=%d"), GridWidth, GridHeight);

    for (int32 y = GridHeight - 1; y >= 0; --y)  // 위에서 아래로 보려고 거꾸로
    {
        FString Row;
        for (int32 x = 0; x < GridWidth; ++x)
        {
            const FSignalRoomCell* Cell = GetCell(x, y);
            TCHAR Symbol = TEXT('.'); // 기본: Empty

            if (Cell)
            {
                switch (Cell->RoomType)
                {
                case ESignalRoomType::Start:      Symbol = TEXT('S'); break;
                case ESignalRoomType::Corridor:   Symbol = TEXT('C'); break;
                case ESignalRoomType::Objective:  Symbol = TEXT('O'); break;
                case ESignalRoomType::Storage:    Symbol = TEXT('T'); break; // s 쓰면 start랑 헷갈리니까 T
                case ESignalRoomType::PowerRoom:  Symbol = TEXT('P'); break;
                case ESignalRoomType::Lab:        Symbol = TEXT('L'); break;
                case ESignalRoomType::Empty:      Symbol = TEXT('.'); break;
                default:                          Symbol = TEXT('?'); break;
                }
            }

            Row.AppendChar(Symbol);
            Row.AppendChar(TEXT(' '));
        }

        UE_LOG(LogTemp, Warning, TEXT("%2d: %s"), y, *Row);
    }

    UE_LOG(LogTemp, Warning, TEXT("==================================="));
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

    // 3) 스폰 포인트 수집 & 분배
    CollectSpawnPoints();
    DistributeAndSpawnItems();
    DistributeAndSpawnEnemies();

    DebugLogGridLayout();

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

    int32 x = 1;
    int32 y = GridHeight / 2;

    if (FSignalRoomCell* StartCell = GetCell(x, y))
    {
        StartCell->RoomType = ESignalRoomType::Start;
    }

    const int32 PathLength = RandomStream.RandRange(5, 8);

    for (int32 i = 0; i < PathLength; ++i)
    {
        // 1) 세로(위/아래로) 먼저 한 칸 움직일지 결정
        int32 DirY = RandomStream.RandRange(-1, 1);   // -1, 0, 1

        if (DirY != 0)
        {
            int32 NewY = FMath::Clamp(y + DirY, 1, GridHeight - 2);
            if (FSignalRoomCell* VerticalCell = GetCell(x, NewY))
            {
                if (VerticalCell->RoomType == ESignalRoomType::Empty)
                {
                    VerticalCell->RoomType = ESignalRoomType::Corridor;
                }
            }
            y = NewY; // y 업데이트
        }

        // 2) 그다음에 항상 오른쪽으로 한 칸
        int32 NewX = FMath::Clamp(x + 1, 1, GridWidth - 2);
        if (FSignalRoomCell* HorizontalCell = GetCell(NewX, y))
        {
            if (HorizontalCell->RoomType == ESignalRoomType::Empty)
            {
                HorizontalCell->RoomType = ESignalRoomType::Corridor;
            }
        }
        x = NewX;
    }

    // 마지막 방은 Objective로 변경
    if (FSignalRoomCell* LastCell = GetCell(x, y))
    {
        LastCell->RoomType = ESignalRoomType::Objective;
    }

    // 사이드 룸 추가 (Storage / PowerRoom)
    AddSideRooms();
    // 문 생성
    BuildDoorConnections();
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
        return;

    for (FSignalRoomCell& Cell : Grid)
    {
        if (Cell.RoomType == ESignalRoomType::Empty)
            continue;


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
            continue;

        const FVector SpawnLocation = GetActorLocation() + FVector(
            Cell.X * CellSize,
            Cell.Y * CellSize,
            0.f
        );

        // 기본값: 회전 없음 (Start/Corridor/Objective 쪽)
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // 단일 입구 방이면 회전만 맞춰서 넣는 전략
        const bool bIsSingleEntranceRoom = Archetype->bIsSingleEntranceRoom;

        if (bIsSingleEntranceRoom)
        {
            const ESignalDoorDirection OpenDir = GetEntranceDirectionForSideRoom(Cell);

            switch (OpenDir)
            {
            case ESignalDoorDirection::North:
                SpawnRotation = FRotator(0.f, 90.f, 0.f);   // +X -> +Y
                break;
            case ESignalDoorDirection::South:
                SpawnRotation = FRotator(0.f, -90.f, 0.f);  // +X -> -Y
                break;
            case ESignalDoorDirection::West:
                SpawnRotation = FRotator(0.f, 0.f, 0.f);    // +X -> +X (West)
                break;
            case ESignalDoorDirection::East:
                SpawnRotation = FRotator(0.f, 180.f, 0.f);  // +X -> -X (East)
                break;
            case ESignalDoorDirection::None:
            default:
                SpawnRotation = FRotator::ZeroRotator;
                break;
            }
        }

        AActor* Spawned = World->SpawnActor<AActor>(RoomClass, SpawnLocation, SpawnRotation);
        if (!Spawned)
            continue;

        // 스폰된 Room Actor를 Cell에 저장 (디버그 & 스폰 포인트 수집 용도)
        Cell.SpawnedRoomActor = Spawned;

        // Start / Corridor / Objective 같은 "문 위치 유동" 타입은 ApplyDoorConfig 사용
        const bool bUsesDoorConfig = !(Archetype->bIsSingleEntranceRoom);
        if (bUsesDoorConfig)
        {
            if (ASignalRoomBase* RoomBase = Cast<ASignalRoomBase>(Spawned))
            {
                RoomBase->ApplyDoorConfig(Cell.Doors);
            }
        }
    }
}

ESignalDoorDirection ASignalFacilityGenerator::GetEntranceDirectionForSideRoom(const FSignalRoomCell& Cell) const
{
    const int32 X = Cell.X;
    const int32 Y = Cell.Y;

    // 위쪽(+Y)이 Corridor면 North
    if (const FSignalRoomCell* N = GetCell(X, Y + 1))
    {
        if (N->RoomType == ESignalRoomType::Corridor)
            return ESignalDoorDirection::North;
    }

    // 왼쪽(+X)이 Corridor면 West
    if (const FSignalRoomCell* E = GetCell(X + 1, Y))
    {
        if (E->RoomType == ESignalRoomType::Corridor)
            return ESignalDoorDirection::West;
    }

    // 아래쪽(-Y)이 Corridor면 South
    if (const FSignalRoomCell* S = GetCell(X, Y - 1))
    {
        if (S->RoomType == ESignalRoomType::Corridor)
            return ESignalDoorDirection::South;
    }

    // 오른쪽(-X)이 Corridor면 East
    if (const FSignalRoomCell* W = GetCell(X - 1, Y))
    {
        if (W->RoomType == ESignalRoomType::Corridor)
            return ESignalDoorDirection::East;
    }

    // 못 찾으면 None
    return ESignalDoorDirection::None;
}


void ASignalFacilityGenerator::BuildDoorConnections()
{
    for (int32 y = 0; y < GridHeight; ++y)
    {
        for (int32 x = 0; x < GridWidth; ++x)
        {
            FSignalRoomCell* Cell = GetCell(x, y);
            if (!Cell || Cell->RoomType == ESignalRoomType::Empty)
                continue;

            FSignalRoomDoors& Doors = Cell->Doors;

            // North(+Y)는 그대로
            if (FSignalRoomCell* North = GetCell(x, y + 1))
            {
                if (North->RoomType != ESignalRoomType::Empty)
                {
                    if (IsSideRoom(Cell->RoomType) && IsSideRoom(North->RoomType))
                    {
                        // skip
                    }
                    else
                    {
                        Doors.bNorth = true;
                        North->Doors.bSouth = true;
                    }
                }
            }

            // West: x+1 (+X가 West)
            if (FSignalRoomCell* West = GetCell(x + 1, y))
            {
                if (West->RoomType != ESignalRoomType::Empty)
                {
                    if (IsSideRoom(Cell->RoomType) && IsSideRoom(West->RoomType))
                    {
                        // skip
                    }
                    else
                    {
                        Doors.bWest = true;
                        West->Doors.bEast = true;
                    }
                }
            }

            // East: x-1 (-X가 East)
            if (FSignalRoomCell* East = GetCell(x - 1, y))
            {
                if (East->RoomType != ESignalRoomType::Empty)
                {
                    if (IsSideRoom(Cell->RoomType) && IsSideRoom(East->RoomType))
                    {
                        // skip
                    }
                    else
                    {
                        Doors.bEast = true;
                        East->Doors.bWest = true;
                    }
                }
            }
        }
    }
}


/**
 * (X, Y) 셀 포인터 반환
 */
FSignalRoomCell* ASignalFacilityGenerator::GetCell(int32 X, int32 Y)
{
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
        return nullptr;

    return &Grid[Y * GridWidth + X];
}

const FSignalRoomCell* ASignalFacilityGenerator::GetCell(int32 X, int32 Y) const
{
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
        return nullptr;

    return &Grid[Y * GridWidth + X];
}

void ASignalFacilityGenerator::CollectSpawnPoints()
{
    ItemSpawnPoints.Empty();
    EnemySpawnPoints.Empty();

    for (const FSignalRoomCell& Cell : Grid)
    {
        if (Cell.RoomType == ESignalRoomType::Empty)
            continue;

        AActor* RoomActor = Cell.SpawnedRoomActor.Get();
        if (!RoomActor)
            continue;


        ASignalRoomBase* RoomBase = Cast<ASignalRoomBase>(RoomActor);
        if (!RoomBase)
            continue;
      
        // 아이템 포인트 수집
        for (USceneComponent* Comp : RoomBase->ItemSpawnPoints)
        {
            if (!Comp) continue;

            FSignalItemSpawnPoint SP;
            SP.RoomType = Cell.RoomType;
            SP.WorldLocation = Comp->GetComponentLocation();
            SP.OwnerRoom = RoomActor;
            SP.PointComponent = Comp;

            ItemSpawnPoints.Add(SP);
        }

        // 적 포인트 수집
        for (USceneComponent* Comp : RoomBase->EnemySpawnPoints)
        {
            if (!Comp) continue;

            FSignalEnemySpawnPoint SP;
            SP.RoomType = Cell.RoomType;
            SP.WorldLocation = Comp->GetComponentLocation();
            SP.OwnerRoom = RoomActor;
            SP.PointComponent = Comp;

            EnemySpawnPoints.Add(SP);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("CollectSpawnPoints: Items=%d, Enemies=%d"),
        ItemSpawnPoints.Num(), EnemySpawnPoints.Num());
}

void ASignalFacilityGenerator::DistributeAndSpawnItems()
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    ASignalGameState* GS = World->GetGameState<ASignalGameState>();
    if (!GS || !GS->StageConfig)
        return;

    USignalItemSet* ItemSet = GS->StageConfig->ItemSet;
    if (!ItemSet || ItemSet->Items.Num() == 0)
        return;

    // 1) 목표 신호량 계산
    const int32 RequiredSignal = GS->StageConfig->RequiredSignal;
    // 맵 전체 신호량은 목표의 1.5~2배 정도를 제공 (여유)
    int32 TargetTotalSignal = static_cast<int32>(RequiredSignal * 1.7f);

    // 2) 스폰 포인트 셔플 (랜덤 순서로 처리)
    TArray<FSignalItemSpawnPoint> ShuffledPoints = ItemSpawnPoints;
    for (int32 i = 0; i < ShuffledPoints.Num(); ++i)
    {
        int32 SwapIdx = RandomStream.RandRange(0, ShuffledPoints.Num() - 1);
        ShuffledPoints.Swap(i, SwapIdx);
    }

    // 3) 스폰 루프
    for (const FSignalItemSpawnPoint& SP : ShuffledPoints)
    {
        if (TargetTotalSignal <= 0)
            break;

        // 이 방(RoomType)에 어울리는 아이템 후보 필터
        TArray<const FSignalItemArchetype*> Candidates;

        for (const FSignalItemArchetype& Item : ItemSet->Items)
        {
            if (Item.PreferredRooms.Num() == 0 || Item.PreferredRooms.Contains(SP.RoomType))
            {
                Candidates.Add(&Item);
            }
        }

        if (Candidates.Num() == 0)
            continue;
        

        // 가중치(SpawnWeight) 기반 랜덤 선택
        int32 TotalWeight = 0;
        for (const FSignalItemArchetype* Item : Candidates)
        {
            TotalWeight += FMath::Max(Item->SpawnWeight, 1);
        }

        int32 R = RandomStream.RandRange(1, TotalWeight);
        const FSignalItemArchetype* ChosenItem = nullptr;

        for (const FSignalItemArchetype* Item : Candidates)
        {
            int32 W = FMath::Max(Item->SpawnWeight, 1);
            if (R <= W)
            {
                ChosenItem = Item;
                break;
            }
            R -= W;
        }

        if (!ChosenItem || !ChosenItem->ItemClass)
            continue;

        // 아이템 스폰
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AActor* SpawnedItem = World->SpawnActor<AActor>(
            ChosenItem->ItemClass,
            SP.WorldLocation,
            FRotator::ZeroRotator,
            Params
        );

        if (ASignalItemActor* ItemActor = Cast<ASignalItemActor>(SpawnedItem))
        {
            ItemActor->InitializeFromArchetype(*ChosenItem);

            // 이 아이템이 기여할 평균 신호량만큼 TargetTotalSignal 감소
            const int32 AvgSignal = (ChosenItem->SignalYieldMin + ChosenItem->SignalYieldMax) / 2;
            TargetTotalSignal -= AvgSignal;
        }
        UE_LOG(LogTemp, Log, TEXT("Spawn Item Location : (%d, %d)"), (int32)SP.WorldLocation.X, (int32)SP.WorldLocation.Y);
    }

    UE_LOG(LogTemp, Log, TEXT("DistributeAndSpawnItems: Remaining TargetSignal=%d"), TargetTotalSignal);

}

void ASignalFacilityGenerator::DistributeAndSpawnEnemies()
{
    // 추후 추가
}
