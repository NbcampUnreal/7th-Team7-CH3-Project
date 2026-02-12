// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KirboStatRow.h"
#include "KirboStatComponent.generated.h"

//UI 갱신용 델리게이트? 공부 더 해야됨
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, float, CurrentValue, float, MaxValue);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM7_CH3_PROJECT_API UKirboStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKirboStatComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Base")
	FKirboStatRow BaseStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Current")
	float CurrentHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Current")
	float CurrentStamina;

	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnStaminaChanged;

	void InitializeStats(const FKirboStatRow& InStatData);

	void TakeDamage(float Amount);
	void Heal(float Amount);
	bool UseStamina(float Amount);
	void RecoverStamina(float Amount);

	float GetMoveSpeed() const { return BaseStat.MoveSpeed; }
	float GetSprintSpeed() const { return BaseStat.MoveSpeed * BaseStat.SprintSpeedMultiplier; }
	float GetDodgeCooldown() const { return BaseStat.DashCooldown; }
	float GetAttackDamageMultiplier() const { return BaseStat.AttackDamageMultiplier; }
};
