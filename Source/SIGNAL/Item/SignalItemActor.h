// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalItemArchetype.h"
#include "SignalItemActor.generated.h"

class USphereComponent;

UCLASS()
class SIGNAL_API ASignalItemActor : public AActor
{
    GENERATED_BODY()

public:
    ASignalItemActor();

    // 아이템 정보(아키타입) 세팅
    void InitializeFromArchetype(const FSignalItemArchetype& Data);

    // 추출 가능한 신호량을 반환
    int32 GetRandomSignalYield() const;

    float GetExtractTime() const { return ExtractTime; }
    float GetNoiseLevel() const { return NoiseLevel; }
    int32 GetSignalGrade() const { return SignalGrade; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<class UStaticMeshComponent> StaticMeshComp;

    // 추출 상호작용 범위 확인용 콜리전
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> InteractionTrigger;

    // 아키타입에서 읽어오는 실제 값들
    UPROPERTY()
    int32 SignalYieldMin;

    UPROPERTY()
    int32 SignalYieldMax;

    UPROPERTY()
    float ExtractTime;

    UPROPERTY()
    float NoiseLevel;

    UPROPERTY()
    int32 SignalGrade;
};

