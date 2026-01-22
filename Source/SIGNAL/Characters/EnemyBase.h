// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Combat,
	Dead
};

class UHealthComponent;
class UTargetingComponent;
class UWeaponComponent;
class UEnemyConfigData;

UCLASS(Abstract)
class SIGNAL_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

    UFUNCTION(BlueprintCallable)
    EEnemyState GetState() const { return State; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

public:	
    virtual void HandleIdle(float DeltaSeconds);
    virtual void HandleCombat(float DeltaSeconds);

    UFUNCTION()
    virtual void OnDeath();

    void SetState(EEnemyState NewState);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPrimitiveComponent* BodyCollision;

    /*UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UHealthComponent* Health;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTargetingComponent* Targeting;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UWeaponComponent* Weapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    UEnemyConfigData* Config;*/

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EEnemyState State = EEnemyState::Idle;

};
