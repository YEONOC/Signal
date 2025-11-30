// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DroneCharacter.h"
#include "GAS/SignalAbilitySystemComponent.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameplayEffect.h"
#include "SignalGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADroneCharacter::ADroneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComp = CreateDefaultSubobject<USignalAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComp->SetIsReplicated(true);

	DroneAttributes = CreateDefaultSubobject<UDroneCoreAttributeSet>(TEXT("DroneAttributes"));

	DroneLight = CreateDefaultSubobject<USpotLightComponent>("DroneLight");
	DroneLight->SetupAttachment(GetRootComponent());
	DroneLight->SetRelativeLocation(FVector(40.f, 0.f, 5.f));
	DroneLight->SetVisibility(false);

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->DefaultLandMovementMode = MOVE_Flying;
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->GravityScale = 0.0f;          // 중력 제거
	}

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		GiveStartupAbilities();

		const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();
		AbilitySystemComp->TryActivateAbilitiesByTag(FGameplayTagContainer(SignalTags.Ability_Drone_PassiveBattery));
	}

}


void ADroneCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 메쉬 기준으로만 기울이기 (캡슐은 그대로)
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	const FVector Vel = GetVelocity();

	// 수평 속도만 사용해서 틸트 계산
	const FVector HorizontalVel(Vel.X, Vel.Y, 0.f);
	const float Speed = HorizontalVel.Size();

	float TargetPitch = 0.f;
	float TargetRoll = 0.f;

	if (Speed > MinSpeedForTilt)
	{
		// 월드 → 로컬 속도 벡터로 변환
		const FRotator ActorRot = GetActorRotation();
		const FVector LocalVel = ActorRot.UnrotateVector(Vel);

		FVector2D LocalDir(LocalVel.X, LocalVel.Y);
		if (!LocalDir.IsNearlyZero())
		{
			LocalDir.Normalize();

			// 전진(앞)일수록 Pitch(-), 후진일수록 Pitch(+)
			TargetPitch = -LocalDir.X * MaxTiltAngle;

			// 오른쪽 이동일수록 Roll(+), 왼쪽 이동일수록 Roll(-)
			TargetRoll = LocalDir.Y * MaxTiltAngle;
		}
	}

	const FRotator CurrentRot = MeshComp->GetRelativeRotation();
	// Yaw는 기존 값 유지 (회전은 컨트롤러가 담당)
	const FRotator TargetRot(TargetPitch, CurrentRot.Yaw, TargetRoll);

	const FRotator NewRot =
		FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, TiltInterpSpeed);

	MeshComp->SetRelativeRotation(NewRot);
}

void ADroneCharacter::HandleMoveInput(const FVector2D& MoveVector)
{
	if (!Controller)
	{
		return;
	}

	// 컨트롤러의 Yaw 기준으로 전/후/좌/우 계산
	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveVector.Y))
	{
		AddMovementInput(ForwardDir, MoveVector.Y);
	}
	if (!FMath::IsNearlyZero(MoveVector.X))
	{
		AddMovementInput(RightDir, MoveVector.X);
	}
}

void ADroneCharacter::HandleUpDownInput(float Value)
{
	if (!Controller)
	{
		return;
	}

	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	const FVector UpDir = FVector::UpVector; // (0,0,1)
	AddMovementInput(UpDir, Value);
}

void ADroneCharacter::HandleStop()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
	}
}

UAbilitySystemComponent* ADroneCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}


void ADroneCharacter::InitializeAttributes()
{
	if (AbilitySystemComp && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComp->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, Context);
		if (SpecHandle.IsValid())
		{
			AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ADroneCharacter::GiveStartupAbilities()
{
	if (!AbilitySystemComp) 
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (*AbilityClass)
		{
			AbilitySystemComp->GiveAbility(
				FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
		}
	}
	// 🔻 여기서 실제 ASC가 가지고 있는 어빌리티 목록 로그 찍기
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComp->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			FString TagString = Spec.DynamicAbilityTags.ToString();

			UE_LOG(LogTemp, Warning, TEXT("ASC Ability: %s | DynamicTags: %s"),
				*Spec.Ability->GetName(),
				*TagString);
		}
	}
}





