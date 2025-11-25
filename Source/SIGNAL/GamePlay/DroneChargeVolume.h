// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "DroneChargeVolume.generated.h"

/**
 * 
 */
UCLASS()
class SIGNAL_API ADroneChargeVolume : public APhysicsVolume
{
	GENERATED_BODY()

public:
	ADroneChargeVolume();

protected:
	virtual void ActorEnteredVolume(AActor* OtherActor) override;
	virtual void ActorLeavingVolume(AActor* OtherActor) override;

	
};
