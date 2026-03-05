// Fill out your copyright notice in the Description page of Project Settings.


#include "Stages/StageSpawner.h"
#include "NavigationSystem.h"
#include "Enemy/IEntityStats.h"
#include "Team7_CH3_Project/Manager/KirboGameState.h"
#include "Kismet/GameplayStatics.h"

void AStageSpawner::InputStageIndex(int32 StageIndexInput)
{
    StageIndex = StageIndexInput;
}

void AStageSpawner::SpawnWave(int32 WaveIndex, FEnemyWaveData WaveData)
{
    RemainingEnemiesInWave = 0;
    for (const FEnemySpawnData& Data : WaveData.SpawnEnemies)
    {
        for (int32 i = 0; i < Data.EnemySpawnCount; i++)
        {
            FVector SpawnLocation = GetRandomNavLocation();

            int32 Retries = 0;
            while (IsInPlayerView(SpawnLocation) && Retries < 1000)
            {
                SpawnLocation = GetRandomNavLocation();
                Retries++;
            }

            AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(Data.EnemyBlueprint, SpawnLocation, FRotator::ZeroRotator);
            if (SpawnedEnemy)
            {
                IEntityStats* Enemy = Cast<IEntityStats>(SpawnedEnemy);
                if (Enemy)
                {
                    RemainingEnemiesInWave++;
                    Enemy->LoadData(StageIndex, WaveIndex);
                }
            }
        }
    }
}

FVector AStageSpawner::GetRandomNavLocation()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation Result;
    if (NavSys && NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), 10000.f, Result))
    {
        return Result.Location;
    }
    return GetActorLocation();
}

bool AStageSpawner::IsInPlayerView(FVector Location)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        FVector2D ScreenLocation;
        if (PC->ProjectWorldLocationToScreen(Location, ScreenLocation))
        {
            int32 ViewportX, ViewportY;
            PC->GetViewportSize(ViewportX, ViewportY);

            return (ScreenLocation.X >= 0 && ScreenLocation.X <= ViewportX && ScreenLocation.Y >= 0 && ScreenLocation.Y <= ViewportY);
        }
    }
    return false;
}

void AStageSpawner::OnEnemyDestroyed()
{
    RemainingEnemiesInWave--;
    if (RemainingEnemiesInWave <= 0)
    {
        AKirboGameState* GS = GetWorld()->GetGameState<AKirboGameState>();
        if (GS) GS->OnWaveCleared();
    }
}
