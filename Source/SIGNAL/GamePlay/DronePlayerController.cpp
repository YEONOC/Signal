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

    // 🔹 상승/하강
    if (IA_UpDown)
    {
        EnhancedInput->BindAction(
            IA_UpDown,
            ETriggerEvent::Triggered,
            this,
            &ADronePlayerController::UpDownInput
        );
    }

    // 2) 어빌리티용 InputConfig 바인딩
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


void ADronePlayerController::MoveInput(const FInputActionValue& Value)
{
    const FVector2D MoveVector = Value.Get<FVector2D>();

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    // 컨트롤러의 바라보는 방향 기준으로 전/후/좌/우 계산
    const FRotator ControlRot = GetControlRotation();
    const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

    const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    if (MoveVector.Y != 0.f)
    {
        ControlledPawn->AddMovementInput(ForwardDir, MoveVector.Y);
    }
    if (MoveVector.X != 0.f)
    {
        ControlledPawn->AddMovementInput(RightDir, MoveVector.X);
    }
}

void ADronePlayerController::LookInput(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();
    UE_LOG(LogTemp, Warning, TEXT("LookInput: X=%.2f Y=%.2f"), LookAxis.X, LookAxis.Y);
    // X: 좌우(Yaw), Y: 상하(Pitch)
    AddYawInput(LookAxis.X);
    AddPitchInput(LookAxis.Y);
}

void ADronePlayerController::UpDownInput(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn || FMath::IsNearlyZero(Axis))
    {
        return;
    }

    // 월드 Z 축 기준으로 위/아래 이동
    const FVector UpDir = FVector::UpVector; // (0,0,1)

    ControlledPawn->AddMovementInput(UpDir, Axis);
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
