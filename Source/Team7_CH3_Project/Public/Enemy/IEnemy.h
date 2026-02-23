// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEnemy.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    Melee UMETA(DisplayName = "Melee"),
    Ranged UMETA(DisplayName = "Ranged")
};

UINTERFACE(MinimalAPI)
class UEnemy : public UInterface
{
	GENERATED_BODY()
};

class TEAM7_CH3_PROJECT_API IEnemy
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() = 0;
	virtual void LoadData(int StageCount = 0, int WaveCount = 0) = 0;

	virtual bool IsLoaded() = 0;

	virtual FString GetName() const = 0;

	virtual EAttackType GetAttackType() const = 0;
	virtual bool HasLineOfSight() const = 0;

	virtual void StartChase() = 0;

	virtual void TryAttack() = 0;
	virtual void TryAction() = 0;

	virtual float GetDamage() const = 0;

	virtual float GetHealth() = 0;
	virtual float GetHealthMax() = 0;
	virtual float GetHealthPercent() = 0;

	virtual void TakeDamage(float DamageAmount) = 0;
	
    virtual float GetAttackRange() const = 0;
	virtual float GetActionRange() const = 0;
	
	virtual bool IsAttackReady() const = 0;
	virtual bool IsActionReady() const = 0;

	virtual float GetMovespeed() const = 0;
    virtual AActor* GetTarget() const = 0;

	virtual int GetGoldDrop() const = 0;
    virtual float GetItemChance() const = 0;

	virtual void Die() = 0;
	virtual bool IsAlive() const = 0;
};
