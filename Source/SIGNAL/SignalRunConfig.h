// SignalRunConfig.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SignalRunConfig.generated.h"

UCLASS(BlueprintType)
class SIGNAL_API USignalRunConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // 한 Run에서 허용되는 Stage 수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxStageCount = 3;

    // Run 클리어를 위한 총 Signal 목표
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 RequiredTotalSignal = 300;
};
