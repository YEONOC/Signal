// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/DronePlayerController.h"
#include "EnhancedInputComponent.h"
#include "GAS/SignalAbilitySystemComponent.h"
#include "GAS/Attributes/DroneCoreAttributeSet.h"
#include "SignalGameplayTags.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "Input/SignalInputConfig.h"
#include "UI/DroneHUDWidget.h"
#include "Characters/DroneCharacter.h"

ADronePlayerController::ADronePlayerController()
{
    bShowMouseCursor = false;
}

void ADronePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 🔹 Enhanced Input MappingContext 등록
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    // HUD 생성
    if (IsLocalController() && DroneHUDClass)
    {
        DroneHUD = CreateWidget<UDroneHUDWidget>(this, DroneHUDClass);
        if (DroneHUD)
        {
            DroneHUD->AddToViewport();

            // ASC & AttributeSet 넘겨주기
            if (ADroneCharacter* Drone = Cast<ADroneCharacter>(GetPawn()))
            {
                if (USignalAbilitySystemComponent* ASC = Drone->FindComponentByClass<USignalAbilitySystemComponent>())
                {
                    const UDroneCoreAttributeSet* AttrSet = ASC->GetSet<UDroneCoreAttributeSet>();
                    DroneHUD->InitializeFromASC(ASC, AttrSet);
                }
            }
        }
    }
}

void ADronePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput) return;
    
    const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

    if (InputConfig)
    {
        for (const FSignalInputAction& ActionEntry : InputConfig->AbilityInputActions)
        {
            if (!ActionEntry.InputAction) continue;

            if (ActionEntry.InputTag == SignalTags.Input_Drone_LightToggle)
            {
                EnhancedInput->BindAction(ActionEntry.InputAction,
                    ETriggerEvent::Started,
                    this,
                    &ADronePlayerController::LightToggleInput);
            }

            // 나중에:
            // else if (ActionEntry.InputTag == Tags.Input_Drone_Scan) { ... }
            // else if (ActionEntry.InputTag == Tags.Input_Drone_Dash) { ... }
        }
    }
    
}


void ADronePlayerController::LightToggleInput(const FInputActionValue& Value)
{
    if (APawn* SignalPawn = GetPawn())
    {
        if (USignalAbilitySystemComponent* ASC =
            SignalPawn->FindComponentByClass<USignalAbilitySystemComponent>())
        {
            const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

            bool bResult = ASC->TryActivateAbilitiesByInputTag(SignalTags.Ability_Drone_LightToggle);
            UE_LOG(LogTemp, Warning, TEXT("TryActivateAbilitiesByInputTag returned: %s"),
                bResult ? TEXT("true") : TEXT("false"));
        }
    }
}
