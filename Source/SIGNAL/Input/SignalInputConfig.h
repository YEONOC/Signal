#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SignalInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FSignalInputAction
{
    GENERATED_BODY()

public:
    // 실제 InputAction 에셋
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    // 이 액션을 나타내는 태그 (예: Input.Drone.LightToggle)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input"))
    FGameplayTag InputTag;
};

UCLASS(BlueprintType)
class SIGNAL_API USignalInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // 어빌리티용 입력 액션들
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TArray<FSignalInputAction> AbilityInputActions;

    // 태그로 InputAction 찾는 헬퍼 (원하면 사용)
    const UInputAction* FindInputActionByTag(const FGameplayTag& InTag) const;
};
