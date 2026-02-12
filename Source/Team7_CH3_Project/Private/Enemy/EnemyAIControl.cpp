// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIControl.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/IEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

void AEnemyAIControl::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    EnemyInterface = Cast<IEnemy>(InPawn);
    if (EnemyInterface)
    {
        EnemyInterface->StartChase();
        GetWorld()->GetTimerManager().SetTimer(AITimer, this, &AEnemyAIControl::UpdateAct, 0.1f, true);
    }
}

void AEnemyAIControl::UpdateAct()
{
    if (!EnemyInterface) return;

    AActor* CurrentTarget = EnemyInterface->GetTarget();
    if (!CurrentTarget) return;

    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), CurrentTarget->GetActorLocation());
    bool bIsRanged = (EnemyInterface->GetAttackType() == EAttackType::Ranged);
    bool bHasLOS = EnemyInterface->HasLineOfSight();

    if (bHasLOS && bIsRanged && Distance <= EnemyInterface->GetAttackRange())
    {
        bIsRepositioning = false;
        StopMovement();
        SetFocus(CurrentTarget);
        EnemyInterface->TryAttack();
        return;
    }

    if (bIsRepositioning)
    {
        if (GetPathFollowingComponent() && GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving)
        {
            return;
        }
        bIsRepositioning = false;
    }

    if (!bIsRanged && Distance <= EnemyInterface->GetAttackRange())
    {
        StopMovement();
        SetFocus(CurrentTarget);
        EnemyInterface->TryAttack();
        return; 
    }
    if (bIsRanged && Distance <= EnemyInterface->GetAttackRange() && !bHasLOS)
    {
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        if (NavSys)
        {
            FVector StrafeDirection = GetPawn()->GetActorRightVector();
            
            FVector GoalPoint = GetPawn()->GetActorLocation() + (StrafeDirection * 250.f);
            
            FNavLocation ProjectedLocation;
            if (NavSys->ProjectPointToNavigation(GoalPoint, ProjectedLocation))
            {
                bIsRepositioning = true;
                MoveToLocation(ProjectedLocation.Location, 50.f);
                return;
            }
        }
    }
    
    if (!EnemyInterface->IsAttackReady()) return;

    bIsRepositioning = false;
    ClearFocus(EAIFocusPriority::Gameplay);
    MoveToActor(CurrentTarget);
}
