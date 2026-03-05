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
        if (StatsInterface->IsActive())
        {
            SetState(EEnemyState::Dead);
        }
        else
        {
            SetState(EEnemyState::Idle);
        }
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

    bool bIsReady = CombatInterface->IsAttackReady();

    if (!bIsReady)
    {
        SetState(EEnemyState::Waiting);
        HandleWaiting(Target);
    }
    else
    {
        float Distance = GetPawn()->GetDistanceTo(Target);
        float Range = CombatInterface->GetAttackRange();
        bool bHasLOS = AIInterface->HasLineOfSight();

        float CurrentZDiff = FMath::Abs(Target->GetActorLocation().Z - GetPawn()->GetActorLocation().Z);
        float AllowedZ = CombatInterface->GetZDifferenceAllowed();

        if (Distance <= Range && bHasLOS && CurrentZDiff <= AllowedZ)
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
    if (!Target || !CombatInterface) return;

    float Range = CombatInterface->GetAttackRange();
    float AllowedZ = CombatInterface->GetZDifferenceAllowed();
    float CurrentZDiff = FMath::Abs(Target->GetActorLocation().Z - GetPawn()->GetActorLocation().Z);

    if (CurrentZDiff > AllowedZ)
    {
        MoveToActor(Target, 0.0f);
        return;
    }

    bool bHasLOS = AIInterface->HasLineOfSight();
    if (CombatInterface->GetAttackType() == EAttackType::Ranged && !bHasLOS)
    {
        FVector EnemyLoc = GetPawn()->GetActorLocation();
        FVector PlayerLoc = Target->GetActorLocation();
        FVector DirToEnemy = (EnemyLoc - PlayerLoc).GetSafeNormal2D();

        for (int32 i = 1; i <= 72; i++)
        {
            float AngleOffset = i * 2.5f;
            float AnglesToTry[] = { AngleOffset, -AngleOffset };

            for (float CurrentAngle : AnglesToTry)
            {
                FVector TestDir = DirToEnemy.RotateAngleAxis(CurrentAngle, FVector::UpVector);
                FVector TestPoint = PlayerLoc + (TestDir * (Range - 100.0f));

                FCollisionQueryParams Params;
                Params.AddIgnoredActor(GetPawn());

                if (!GetWorld()->LineTraceTestByChannel(
                    TestPoint + FVector(0, 0, 60.f),
                    PlayerLoc + FVector(0, 0, 60.f),
                    ECC_Visibility, Params))
                {
                    float BufferAngle = (CurrentAngle > 0) ? (CurrentAngle + 7.5f) : (CurrentAngle - 7.5f);

                    FVector BufferedDir = DirToEnemy.RotateAngleAxis(BufferAngle, FVector::UpVector);
                    FVector FinalPoint = PlayerLoc + (BufferedDir * (Range - 100.0f));

                    MoveToLocation(FinalPoint, 5.0f);
                    return;
                }
            }
        }
    }

    MoveToActor(Target, 25.0f);
}

void AEnemyAIControl::HandleAttacking(AActor* Target)
{
    if (CombatInterface->IsAttackReady())
    {
        StopMovement();
        CombatInterface->TryAttack();
    }
}

void AEnemyAIControl::HandleWaiting(AActor* Target)
{
    if (CombatInterface->IsAiming())
    {
        SetFocus(Target);
    }
    else
    {
        ClearFocus(EAIFocusPriority::Gameplay);
    }
    StopMovement();
}

void AEnemyAIControl::SetState(EEnemyState NewState)
{
    if (CurrentState == NewState) return;
    CurrentState = NewState;
}
