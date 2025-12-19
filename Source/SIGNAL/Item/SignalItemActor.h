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

    // 정보 (범위 내 랜덤) 추출
    UFUNCTION(BlueprintCallable, Category = "Signal")
    int32 ExtractSignal();
    UFUNCTION(BlueprintPure, Category = "Signal")
    int32 GetSignalYieldMin() const { return SignalYieldMin; }
    int32 GetSignalYieldMax() const { return SignalYieldMax; }
    float GetExtractTime() const { return ExtractTime; }
    float GetNoiseLevel() const { return NoiseLevel; }
    int32 GetSignalGrade() const { return SignalGrade; }
    FName GetItemName() const { return ItemName; }

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan")
    TObjectPtr<class UScanHighlightComponent> ScanHighlight;

protected:
    // ---------- Item 정보 -----------
    // 추출 상호작용 범위 확인용 콜리전
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<class UStaticMeshComponent> StaticMeshComp;

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

    // 추출 상호작용 범위 확인용 콜리전
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> InteractionTrigger;

    // 중복 추출 방지용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Signal")
    bool bExtracted = false;
};

