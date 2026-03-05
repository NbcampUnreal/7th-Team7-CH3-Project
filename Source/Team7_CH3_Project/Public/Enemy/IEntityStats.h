// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEntityStats.generated.h"

UINTERFACE(MinimalAPI)
class UEntityStats : public UInterface { GENERATED_BODY() };

class TEAM7_CH3_PROJECT_API IEntityStats
{
	GENERATED_BODY()

public:
    virtual void LoadData(int StageCount = 0, int WaveCount = 0) = 0;

    virtual float GetHealth() const = 0;
    virtual float GetHealthMax() const = 0;
    virtual float GetHealthPercent() const = 0;

    virtual bool IsActive() const = 0;
    virtual void TakeDamage(float DamageAmount) = 0;
    virtual bool IsAlive() const = 0;
    virtual void Die() = 0;
};
