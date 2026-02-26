// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageData.h"
#include "StageSpawner.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AStageSpawner : public AActor
{
	GENERATED_BODY()

public:
    void SpawnWave(FEnemyWaveData WaveData);

    void OnEnemyDestroyed();

private:
    int32 RemainingEnemiesInWave = 0;

    FVector GetRandomNavLocation();
    bool IsInPlayerView(FVector Location);
};
