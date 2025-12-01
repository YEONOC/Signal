#include "SignalRoomBase.h"
#include "../SignalRoomTypes.h"

ASignalRoomBase::ASignalRoomBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASignalRoomBase::ApplyDoorConfig_Implementation(const FSignalRoomDoors& Doors)
{
    // C++에서는 기본 구현은 비워두고,
    // Blueprint에서 Override해서 문 Mesh On/Off 처리
}
