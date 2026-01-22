// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemySentryDrone.generated.h"

class UCapsuleComponent;
class USpotLightComponent;
class UScanHighlightComponent;
class ADroneCharacter;

UCLASS()
class SIGNAL_API AEnemySentryDrone : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemySentryDrone();

protected:
	virtual void BeginPlay() override;



private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpotLightComponent> SpotLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UScanHighlightComponent> ScanHighlight;


};
