// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalExitActor.generated.h"

UCLASS()
class SIGNAL_API ASignalExitActor : public AActor
{
    GENERATED_BODY()

public:
    ASignalExitActor();

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class USphereComponent> InteractSphere;

    UPROPERTY()
    TWeakObjectPtr<class ADroneCharacter> OverlappedDrone;

    UFUNCTION()
    void OnBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

public:
    void TryExit(); // 컨트롤러 입력에서 호출
};
