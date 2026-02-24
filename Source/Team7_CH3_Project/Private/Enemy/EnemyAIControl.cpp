// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIControl.h"
#include "Enemy/IAIBehavior.h"
#include "Enemy/ICombatEntity.h"
#include "Enemy/IEntityStats.h"
#include "Navigation/PathFollowingComponent.h"

void AEnemyAIControl::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AIInterface = Cast<IAIBehavior>(InPawn);
    CombatInterface = Cast<ICombatEntity>(InPawn);
    StatsInterface = Cast<IEntityStats>(InPawn);

    if (AIInterface)
    {
        AIInterface->StartAct();
        GetWorld()->GetTimerManager().SetTimer(AITimer, this, &AEnemyAIControl::UpdateAct, 0.1f, true);
    }
}

void AEnemyAIControl::UpdateAct()
{
    if (StatsInterface && (!StatsInterface->IsAlive() || !StatsInterface->IsActive()))
    {
        SetState(EEnemyState::Dead);
        GetWorldTimerManager().ClearTimer(AITimer);
        StopMovement();
        return;
    }

    if (!AIInterface || !CombatInterface) return;

    AActor* Target = AIInterface->GetTarget();
    if (!Target)
    {
        SetState(EEnemyState::Idle);
        return;
    }

    bool bIsWaiting = CombatInterface->IsAttackReady();

    if (!bIsWaiting)
    {
        SetState(EEnemyState::Waiting);
        HandleWaiting(Target);
    }
    else
    {
        float Distance = GetPawn()->GetDistanceTo(Target);
        float Range = CombatInterface->GetAttackRange();
        bool bHasLOS = AIInterface->HasLineOfSight();

        if (Distance <= Range && bHasLOS)
        {
            SetState(EEnemyState::Attacking);
            HandleAttacking(Target);
        }
        else
        {
            SetState(EEnemyState::Chasing);
            HandleChasing(Target);
        }
    }
}

void AEnemyAIControl::HandleChasing(AActor* Target)
{
    ClearFocus(EAIFocusPriority::Gameplay);

    float Acceptance = 25.0f;
    MoveToActor(Target, Acceptance);
}

void AEnemyAIControl::HandleAttacking(AActor* Target)
{
    SetFocus(Target);

    if (CombatInterface->IsAttackReady())
    {
        StopMovement();
        CombatInterface->TryAttack();
    }
}

void AEnemyAIControl::HandleWaiting(AActor* Target)
{
    ClearFocus(EAIFocusPriority::Gameplay);
    StopMovement();
}

void AEnemyAIControl::SetState(EEnemyState NewState)
{
    if (CurrentState == NewState) return;
    CurrentState = NewState;
}
