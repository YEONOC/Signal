// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneChargeVolume.h"
#include "Characters/DroneCharacter.h"
#include "AbilitySystemComponent.h"
#include "SignalGameplayTags.h"

ADroneChargeVolume::ADroneChargeVolume()
{
	//bPainCausing = false;
}

void ADroneChargeVolume::ActorEnteredVolume(AActor* OtherActor)
{
	Super::ActorEnteredVolume(OtherActor);

	if (ADroneCharacter* Drone = Cast<ADroneCharacter>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = Drone->GetAbilitySystemComponent())
		{
			const auto& SignalTags = FSignalGameplayTags::Get();
			ASC->AddLooseGameplayTag(SignalTags.State_Drone_Charging);
#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Warning, TEXT("Drone entered Charging Volume"));
#endif
		}
	}
}

void ADroneChargeVolume::ActorLeavingVolume(AActor* OtherActor)
{
	Super::ActorLeavingVolume(OtherActor);

	if (ADroneCharacter* Drone = Cast<ADroneCharacter>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = Drone->GetAbilitySystemComponent())
		{
			const auto& SignalTags = FSignalGameplayTags::Get();
			ASC->RemoveLooseGameplayTag(SignalTags.State_Drone_Charging);

#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Warning, TEXT("Drone left Charging Volume"));
#endif
		}
	}
}
