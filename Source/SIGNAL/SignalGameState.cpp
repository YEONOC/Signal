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

    // Stage 클리어는 Exit로 탈출할 때만 발생
    // Signal은 단순히 수집만 하고, Run 누적에 사용됨
}

bool ASignalGameState::IsStageCleared() const
{
    if (!StageConfig)
    {
        return false;
    }

    // 정확히 TargetSignalSupply에 도달했는지 확인
    return CurrentSignal >= StageConfig->TargetSignalSupply;
}
