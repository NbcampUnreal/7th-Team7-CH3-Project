// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAIBehavior.generated.h"

UINTERFACE(MinimalAPI)
class UAIBehavior : public UInterface { GENERATED_BODY() };

class TEAM7_CH3_PROJECT_API IAIBehavior
{
	GENERATED_BODY()

public:
    virtual void StartAct() = 0;
    virtual AActor* GetTarget() const = 0;
    virtual bool HasLineOfSight() const = 0;
    virtual float GetMovespeed() const = 0;
};
