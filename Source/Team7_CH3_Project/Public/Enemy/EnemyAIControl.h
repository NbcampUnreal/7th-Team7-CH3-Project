// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIControl.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AEnemyAIControl : public AAIController
{
	GENERATED_BODY()

protected:
    bool bIsRepositioning = false;
    FVector LastRepositionLocation;

    virtual void OnPossess(APawn* InPawn) override;
    
    void UpdateAct();

    FTimerHandle AITimer;
    class IEnemy* EnemyInterface;
};
