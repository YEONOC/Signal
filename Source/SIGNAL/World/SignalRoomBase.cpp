#include "SignalRoomBase.h"
#include "Components/ArrowComponent.h"
#include "../SignalRoomTypes.h"

ASignalRoomBase::ASignalRoomBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASignalRoomBase::BeginPlay()
{
    Super::BeginPlay();

    TArray<UArrowComponent*> Arrows;
    GetComponents<UArrowComponent>(Arrows);

    for (USceneComponent* Arrow : Arrows)
    {
        if (Arrow->ComponentHasTag("ItemPoint"))
        {
            ItemSpawnPoints.Add(Arrow);
        }
        else if (Arrow->ComponentHasTag("EnemyPoint"))
        {
            EnemySpawnPoints.Add(Arrow);
        }
    }
}

void ASignalRoomBase::ApplyDoorConfig_Implementation(const FSignalRoomDoors& Doors)
{
    // C++에서는 기본 구현은 비워두고,
    // Blueprint에서 Override해서 문 Mesh On/Off 처리
}
