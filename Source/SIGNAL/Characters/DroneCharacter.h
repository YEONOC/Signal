// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "DroneCharacter.generated.h"

class USignalAbilitySystemComponent;
class UDroneCoreAttributeSet;
class UGameplayEffect;
class USpotLightComponent;

UCLASS()
class SIGNAL_API ADroneCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADroneCharacter();
    void InitializeAttributes();
    void GiveStartupAbilities();

    virtual void Tick(float DeltaSeconds) override;

    // IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    UPROPERTY()
    TObjectPtr<USpotLightComponent> DroneLight;

    // 시작 시 적용될 GameplayAbility
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

    // DroneCharacter.h
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<class UGameplayEffect> BatteryDeltaEffect;

    // 이동 입력 처리 함수 (PC에서 호출)
    void HandleMoveInput(const FVector2D& MoveVector);
    void HandleUpDownInput(float Value);
    void HandleStop();

protected:
    // GAS Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<USignalAbilitySystemComponent> AbilitySystemComp;

    // AttributeSet
    UPROPERTY()
    TObjectPtr<UDroneCoreAttributeSet> DroneAttributes;

    // 시작 시 적용할 기본 스탯 GameplayEffect
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

    // --- Drone Tilt Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone|Tilt")
    float MaxTiltAngle = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone|Tilt")
    float TiltInterpSpeed = 6.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone|Tilt")
    float MinSpeedForTilt = 5.f;

    // 틸트 기준이 되는 컴포넌트 (Capsule 아래)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone")
    TObjectPtr<USceneComponent> BodyRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone")
    TObjectPtr<USkeletalMeshComponent> DroneMesh;

};
