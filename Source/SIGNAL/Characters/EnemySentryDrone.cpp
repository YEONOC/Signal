// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemySentryDrone.h"

#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "Characters/DroneCharacter.h"
#include "Compo/ScanHighlightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "SignalGameplayTags.h"
#include "DrawDebugHelpers.h"

AEnemySentryDrone::AEnemySentryDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	SpotLight = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	if (GetMesh())
	{
		SpotLight->SetupAttachment(GetMesh());
	}
	else
	{
		SpotLight->SetupAttachment(Root);
	}
	SpotLight->SetVisibility(true);
	SpotLight->SetRelativeLocation(FVector(40.f, 0.f, 10.f));

	ScanHighlight = CreateDefaultSubobject<UScanHighlightComponent>("ScanHighlight");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemySentryDrone::BeginPlay()
{
	Super::BeginPlay();

	if (ScanHighlight)
	{
		ScanHighlight->SetSignalGrade(3);
	}

}

