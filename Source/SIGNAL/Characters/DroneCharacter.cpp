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
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComp = CreateDefaultSubobject<USignalAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComp->SetIsReplicated(true);

	DroneAttributes = CreateDefaultSubobject<UDroneCoreAttributeSet>(TEXT("DroneAttributes"));

	DroneLight = CreateDefaultSubobject<USpotLightComponent>("DroneLight");
	DroneLight->SetupAttachment(GetRootComponent());
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



