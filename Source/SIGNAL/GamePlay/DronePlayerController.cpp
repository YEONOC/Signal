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

    // 1) 이동/카메라 먼저 바인딩
    if (IA_Move)
    {
        EnhancedInput->BindAction(
            IA_Move,
            ETriggerEvent::Triggered,          // 방향키는 계속 누르고 있을 때 계속 호출
            this,
            &ADronePlayerController::MoveInput
        );
    }

    if (IA_Look)
    {
        EnhancedInput->BindAction(
            IA_Look,
            ETriggerEvent::Triggered,
            this,
            &ADronePlayerController::LookInput
        );
    }

    // 상승/하강
    if (IA_UpDown)
    {
        EnhancedInput->BindAction(
            IA_UpDown,
            ETriggerEvent::Triggered,
            this,
            &ADronePlayerController::UpDownInput
        );
    }

    // 정지
    if (IA_Stop)
    {
        EnhancedInput->BindAction(
            IA_Stop,
            ETriggerEvent::Started,
            this,
            &ADronePlayerController::StopInput
        );
    }

    // 2) 어빌리티용 InputConfig 바인딩
    if (InputConfig)
    {
        for (const FSignalInputAction& ActionEntry : InputConfig->AbilityInputActions)
        {
            if (!ActionEntry.InputAction) continue;

            // Light Toggle
            if (ActionEntry.InputTag == SignalTags.Input_Drone_LightToggle)
            {
                EnhancedInput->BindAction(ActionEntry.InputAction,
                    ETriggerEvent::Started,
                    this,
                    &ADronePlayerController::LightToggleInput);
            }
            // Scan
            else if (ActionEntry.InputTag == SignalTags.Input_Drone_Scan)
            {
                EnhancedInput->BindAction(
                    ActionEntry.InputAction,
                    ETriggerEvent::Started,
                    this,
                    &ADronePlayerController::ScanInput
                );
            }
            // Extract
            else if (ActionEntry.InputTag == SignalTags.Input_Drone_Extract)
            {
                // 누르기 시작 → 어빌리티 활성
                EnhancedInput->BindAction(
                    ActionEntry.InputAction,
                    ETriggerEvent::Started,
                    this,
                    &ADronePlayerController::ExtractInput_Pressed);

                // 떼는 순간 → 어빌리티 Cancel
                EnhancedInput->BindAction(
                    ActionEntry.InputAction,
                    ETriggerEvent::Completed, // 또는 Canceled
                    this,
                    &ADronePlayerController::ExtractInput_Released);
            }
            // 나중에:
            // else if (ActionEntry.InputTag == Tags.Input_Drone_Dash) { ... }
        }
    }
    
}


void ADronePlayerController::MoveInput(const FInputActionValue& Value)
{
    const FVector2D MoveVector = Value.Get<FVector2D>();

    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(GetPawn()))
    {
        Drone->HandleMoveInput(MoveVector);
    }
}

void ADronePlayerController::LookInput(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();
    // UE_LOG(LogTemp, Warning, TEXT("LookInput: X=%.2f Y=%.2f"), LookAxis.X, LookAxis.Y);

    // X: 좌우(Yaw), Y: 상하(Pitch)
    AddYawInput(LookAxis.X);
    AddPitchInput(LookAxis.Y);

}

void ADronePlayerController::UpDownInput(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();

    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(GetPawn()))
    {
        Drone->HandleUpDownInput(Axis);
    }
}

void ADronePlayerController::StopInput(const FInputActionValue& Value)
{
    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(GetPawn()))
    {
        Drone->HandleStop();
    }
}

void ADronePlayerController::LightToggleInput(const FInputActionValue& Value)
{
    if (APawn* SignalPawn = GetPawn())
    {
        if (USignalAbilitySystemComponent* ASC = SignalPawn->FindComponentByClass<USignalAbilitySystemComponent>())
        {
            const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

            bool bResult = ASC->TryActivateAbilitiesByInputTag(SignalTags.Input_Drone_LightToggle);
            UE_LOG(LogTemp, Warning, TEXT("LightToggleInput: TryActivateAbilitiesByTag = %s"), bResult ? TEXT("true") : TEXT("false"));
        }
    }
}

void ADronePlayerController::ScanInput(const FInputActionValue& Value)
{
    if (APawn* SignalPawn = GetPawn())
    {
        if (USignalAbilitySystemComponent* ASC = SignalPawn->FindComponentByClass<USignalAbilitySystemComponent>())
        {
            const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

            bool bResult = ASC->TryActivateAbilitiesByInputTag(SignalTags.Input_Drone_Scan);

            UE_LOG(LogTemp, Warning, TEXT("ScanInput: TryActivateAbilitiesByTag = %s"), bResult ? TEXT("true") : TEXT("false"));
        }
    }
}

void ADronePlayerController::ExtractInput_Pressed(const FInputActionValue& Value)
{
    if (APawn* SignalPawn = GetPawn())
    {
        if (USignalAbilitySystemComponent* ASC = SignalPawn->FindComponentByClass<USignalAbilitySystemComponent>())
        {
            const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();
            
            bool bResult = ASC->TryActivateAbilitiesByInputTag(SignalTags.Input_Drone_Extract);

            UE_LOG(LogTemp, Warning, TEXT("ExtractInput: TryActivateAbilitiesByTag = %s"), bResult ? TEXT("true") : TEXT("false"));
        }
    }
}

void ADronePlayerController::ExtractInput_Released(const FInputActionValue& Value)
{
    if (APawn* SignalPawn = GetPawn())
    {
        if (USignalAbilitySystemComponent* ASC = SignalPawn->FindComponentByClass<USignalAbilitySystemComponent>())
        {
            const FSignalGameplayTags& SignalTags = FSignalGameplayTags::Get();

            ASC->CancelAbilitiesByInputTag(SignalTags.Input_Drone_Extract);

            UE_LOG(LogTemp, Warning, TEXT("ExtractInput: CancelAbilitiesByTag"));
        }
    }
}
