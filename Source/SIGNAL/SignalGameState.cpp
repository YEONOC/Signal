// SignalGameState.cpp

#include "SignalGameState.h"
#include "Gameplay/SignalStageConfig.h"

ASignalGameState::ASignalGameState()
{
    CurrentSignal = 0;
}

void ASignalGameState::InitializeSignalFromStage()
{
    CurrentSignal = 0;

    // HUD 초기화 용도로 브로드캐스트
    OnSignalAmountChanged.Broadcast(CurrentSignal);
}

void ASignalGameState::AddSignal(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    CurrentSignal += Amount;

    OnSignalAmountChanged.Broadcast(CurrentSignal);

    if (IsStageCleared())
    {
        HandleStageCleared();
    }
}

bool ASignalGameState::IsStageCleared() const
{
    if (!StageConfig)
    {
        return false;
    }

    return CurrentSignal >= StageConfig->RequiredSignal;
}

int32 ASignalGameState::GetRequiredSignal() const
{
    return StageConfig ? StageConfig->RequiredSignal : 0;
}

int32 ASignalGameState::GetBonusSignalTarget() const
{
    return StageConfig ? StageConfig->BonusSignalTarget : 0;
}

void ASignalGameState::HandleStageCleared()
{
    OnStageCleared.Broadcast();
    // 실제 클리어 처리(연출/다음 스테이지 이동)는 GameMode/HUD에서 처리
}
