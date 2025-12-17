// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/SignalExitActor.h"
#include "Characters/DroneCharacter.h"
#include "SIGNALGameMode.h"

ASignalExitActor::ASignalExitActor()
{
}

void ASignalExitActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(OtherActor))
    {
        Drone->SetCurrentExit(this);
    }
}

void ASignalExitActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(OtherActor))
    {
        if (Drone->GetCurrentExit() == this)
        {
            Drone->SetCurrentExit(nullptr);
        }
    }
}

void ASignalExitActor::TryExit()
{
    if (!OverlappedDrone.IsValid()) return;

    if (UWorld* World = GetWorld())
    {
        if (ASIGNALGameMode* GM = World->GetAuthGameMode<ASIGNALGameMode>())
        {
            GM->EndStage(true);
        }
    }
}

