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
	// 위/아래 회전도 카메라(컨트롤러) 따라가기
	bUseControllerRotationPitch = true;
}

// Called when the game starts or when spawned
void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADroneCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// PlayerController가 Pawn을 Possess할 때 Ability 부여
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

	if (Controller)
	{
		FRotator ControlRot = Controller->GetControlRotation();

		// Roll은 0으로 두고, Pitch/Yaw만 따라가게
		FRotator NewRot(ControlRot.Pitch, ControlRot.Yaw, 0.f);
		SetActorRotation(NewRot);
	}

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
		const FRotator ActorRot = GetActorRotation();
		const FVector LocalVel = ActorRot.UnrotateVector(Vel);

		FVector2D LocalDir(LocalVel.X, LocalVel.Y);
		if (!LocalDir.IsNearlyZero())
		{
			LocalDir.Normalize();

			TargetPitch = -LocalDir.X * MaxTiltAngle;

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

	// 전진/후진 : 카메라가 실제로 바라보는 방향 (Pitch 포함)
	FVector ForwardDir = ControlRot.Vector();
	ForwardDir = ForwardDir.GetSafeNormal();

	// 좌/우 이동 : 카메라 Right에서 수평만 사용 (Z=0)
	FVector RightDir = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
	RightDir.Z = 0.f;
	RightDir = RightDir.GetSafeNormal();

	if (!ForwardDir.IsNearlyZero() && MoveVector.Y != 0.f)
	{
		AddMovementInput(ForwardDir, MoveVector.Y);
	}

	if (!RightDir.IsNearlyZero() && MoveVector.X != 0.f)
	{
		AddMovementInput(RightDir, MoveVector.X);
	}

	// 이전 버전 : 상승 하강 조작 버전
	/*const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveVector.Y))
	{
		AddMovementInput(ForwardDir, MoveVector.Y);
	}
	if (!FMath::IsNearlyZero(MoveVector.X))
	{
		AddMovementInput(RightDir, MoveVector.X);
	}*/
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
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComp->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			FString TagString = Spec.DynamicAbilityTags.ToString();
#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Warning, TEXT("ASC Ability: %s | DynamicTags: %s"), *Spec.Ability->GetName(), *TagString);
#endif
		}
	}
}





