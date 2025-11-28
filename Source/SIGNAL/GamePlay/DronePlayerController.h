// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "DronePlayerController.generated.h"

class UInputAction;
class USignalInputConfig;
class UInputMappingContext;
class UDroneHUDWidget;

/**
 * 
 */
UCLASS()
class SIGNAL_API ADronePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ADronePlayerController();

protected:
    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;

    /** 드론 기본 입력 매핑 컨텍스트 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /** Input Action 목록 */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<USignalInputConfig> InputConfig;

    // 이동/카메라용 InputAction
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Move;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Look;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_UpDown;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Stop;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDroneHUDWidget> DroneHUDClass;

    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);
    void UpDownInput(const FInputActionValue& Value);
    void StopInput(const FInputActionValue& Value);

	void LightToggleInput(const FInputActionValue& Value);
    void ScanInput(const FInputActionValue& Value);

private:
    UPROPERTY()
    TObjectPtr<UDroneHUDWidget> DroneHUD;
	
};
