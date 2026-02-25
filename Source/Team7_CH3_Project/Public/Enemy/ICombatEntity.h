// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICombatEntity.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged")
};

UINTERFACE(MinimalAPI)
class UCombatEntity : public UInterface { GENERATED_BODY() };

class TEAM7_CH3_PROJECT_API ICombatEntity
{
	GENERATED_BODY()

public:
    virtual EAttackType GetAttackType() const = 0;
    virtual float GetDamage() const = 0;

    virtual void TryAttack() = 0;
    virtual bool IsAttackReady() const = 0;
    virtual float GetAttackRange() const = 0;
    virtual float GetAttackCooldown() const = 0;
    virtual float GetZDifferenceAllowed() const = 0;

    virtual bool IsAiming() const = 0;
    virtual void ExecuteAction(int32 ActionID) = 0;
};
