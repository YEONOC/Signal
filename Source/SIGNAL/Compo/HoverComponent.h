#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoverComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIGNAL_API UHoverComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHoverComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        enum ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // 호버 세기 (cm)
    UPROPERTY(EditAnywhere, Category = "Hover")
    float Amplitude = 10.f;

    // 호버 속도 (rad/sec)
    UPROPERTY(EditAnywhere, Category = "Hover")
    float Speed = 1.5f;

    // X/Y도 살짝 흔들릴지
    UPROPERTY(EditAnywhere, Category = "Hover")
    bool bAffectXY = false;

    UPROPERTY(EditAnywhere, Category = "Hover")
    float XYAmplitude = 3.f;

    // 랜덤 위상값 사용
    UPROPERTY(EditAnywhere, Category = "Hover")
    bool bUseRandomPhase = true;

private:
    FVector BaseLocation;
    float Phase = 0.f;
    bool bInitialized = false;
};