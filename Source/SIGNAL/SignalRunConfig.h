#pragma once

#include "CoreMinimal.h"
#include "SignalRunConfig.generated.h"

class USignalEnemySet;
class USignalItemSet;

USTRUCT(BlueprintType)
struct FSignalRunConfig
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere)
    int32 ThreatLevel;        // 적 난이도

    UPROPERTY(EditAnywhere)
    int32 MapSize;            // 맵 크기/복잡도 (간단히)

    UPROPERTY(EditAnywhere)
    int32 RequiredSignal;     // 스테이지 클리어에 필요한 신호 정보량

    UPROPERTY(EditAnywhere)
    int32 BonusSignalTarget;  // 이 이상 모으면 추가 보상 조건

    UPROPERTY(EditAnywhere)
    TSubclassOf<USignalItemSet> ItemSet;   // 이 맵에서 사용할 아이템 풀

    //UPROPERTY(EditAnywhere)
    //TSubclassOf<USignalEnemySet> EnemySet; // 이 맵에서 사용할 적 풀
};