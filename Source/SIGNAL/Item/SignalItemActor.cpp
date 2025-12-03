// Fill out your copyright notice in the Description page of Project Settings.

#include "SignalItemActor.h"
#include "Components/SphereComponent.h"

ASignalItemActor::ASignalItemActor()
{
    InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
    RootComponent = InteractionTrigger;
    InteractionTrigger->InitSphereRadius(60.f);
}

void ASignalItemActor::InitializeFromArchetype(const FSignalItemArchetype& Data)
{
    SignalYieldMin = Data.SignalYieldMin;
    SignalYieldMax = Data.SignalYieldMax;
    ExtractTime = Data.ExtractTime;
    NoiseLevel = Data.NoiseLevel;
    SignalGrade = Data.SignalGrade;
}

int32 ASignalItemActor::GetRandomSignalYield() const
{
    return FMath::RandRange(SignalYieldMin, SignalYieldMax);
}


