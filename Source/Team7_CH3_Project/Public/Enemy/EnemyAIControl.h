// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIControl.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle,
    Chasing,
    Attacking,
    Waiting,
    Dead
};

UCLASS()
class TEAM7_CH3_PROJECT_API AEnemyAIControl : public AAIController
{
    GENERATED_BODY()

protected:
    class IAIBehavior* AIInterface;
    class ICombatEntity* CombatInterface;
    class IEntityStats* StatsInterface;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    EEnemyState CurrentState = EEnemyState::Idle;

    FTimerHandle AITimer;

    virtual void OnPossess(APawn* InPawn) override;

    void UpdateAct();

    void HandleChasing(AActor* Target);
    void HandleAttacking(AActor* Target);
    void HandleWaiting(AActor* Target);

public:
    void SetState(EEnemyState NewState);
};
