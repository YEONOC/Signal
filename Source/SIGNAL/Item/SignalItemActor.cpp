// Fill out your copyright notice in the Description page of Project Settings.

#include "SignalItemActor.h"
#include "GameFramework/Actor.h"
#include "SignalItemArchetype.h"
#include "Compo/ScanHighlightComponent.h"
#include "Components/SphereComponent.h"

ASignalItemActor::ASignalItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    RootComponent = StaticMeshComp;

    StaticMeshComp->SetCollisionObjectType(ECC_GameTraceChannel1);

    // StaticMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
    // StaticMeshComp->SetGenerateOverlapEvents(true);

    InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
    InteractionTrigger->InitSphereRadius(60.f);

    ScanHighlight = CreateDefaultSubobject<UScanHighlightComponent>(TEXT("ScanHighlight"));
}


void ASignalItemActor::BeginPlay()
{
    Super::BeginPlay();
 
}


void ASignalItemActor::InitializeFromArchetype(const FSignalItemArchetype& Data)
{
    SignalYieldMin = Data.SignalYieldMin;
    SignalYieldMax = Data.SignalYieldMax;
    ExtractTime = Data.ExtractTime;
    NoiseLevel = Data.NoiseLevel;
    SignalGrade = Data.SignalGrade;

    // 비주얼 적용
    if (Data.Mesh)
    {
        StaticMeshComp->SetStaticMesh(Data.Mesh);
    }
    if (Data.OverrideMaterial)
    {
        StaticMeshComp->SetMaterial(0, Data.OverrideMaterial);
    }

    ScanHighlight->SetSignalGrade(SignalGrade);
}

int32 ASignalItemActor::ExtractSignal()
{
    if (bExtracted)
        return 0;

    bExtracted = true;

    const int SignalAmount = FMath::RandRange(SignalYieldMin, SignalYieldMax);
    
    Destroy();

    return SignalAmount;
}

int32 ASignalItemActor::GetRandomSignalYield() const
{
    return FMath::RandRange(SignalYieldMin, SignalYieldMax);
}


