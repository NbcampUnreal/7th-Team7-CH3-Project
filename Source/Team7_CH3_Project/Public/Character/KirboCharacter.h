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
class UAnimMontage;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	FTimerHandle InvincibilityTimerHandle;

	bool bIsInvincible = false; // 무적 상태 확인
	bool bCanDash = true;
	bool bIsShooting = false;
	bool bIsControlEnabled = false;

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

	void ResetInvincibility();
	void HandleDeath();


public:
	UPROPERTY()
	UMaterialInstanceDynamic* StaminaMaterialDynamic;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UStaticMeshComponent* StaminaPlaneComp;

	void UpdateStamina(float CurrentStamina, float MaxStamina);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void TestSelfDamage(); // KH 260224 추가 : 플로팅 텍스트 테스트용 함수 선언
};
