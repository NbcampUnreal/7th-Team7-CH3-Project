// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "KirboCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UKirboStatComponent;
class UDataTable;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TEAM7_CH3_PROJECT_API AKirboCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AKirboCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	UKirboStatComponent* StatComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	UDataTable* CharacterStatTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ShotAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	FTimerHandle DashTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float DashCooldownTime = 2.0f;
	bool bCanDash = true;
	bool bIsShooting = false;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void EnablePlayerInput();

	void Move(const FInputActionValue& Value);
	void StartShot();
	void StopShot();
	void StartSprint();
	void StopSprint();
	void Dash();
	void ResetDash();

    
public: // KH 추가 (시작)
    UPROPERTY()
    UMaterialInstanceDynamic* StaminaMaterialDynamic; // 머티리얼을 제어할 변수 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UStaticMeshComponent* StaminaPlaneComp; // 스태미나 바를 표시할 발밑의 판
    
    void UpdateStamina(float CurrentStamina, float MaxStamina); // 외부나 Tick에서 호출할 함수 선언
    // KH 추가 (끝)
};
