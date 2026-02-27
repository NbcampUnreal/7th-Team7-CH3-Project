// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageData.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    TSubclassOf<AActor> EnemyBlueprint;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 EnemySpawnCount;
};
USTRUCT(BlueprintType)
struct FEnemyWaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnData")
    TArray<FEnemySpawnData> SpawnEnemies;
};

USTRUCT(BlueprintType)
struct FStageData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData")
    FName LevelName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
    float StageStartDelay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
    float StageEndDelay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveData")
    float WavesDelay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveData")
    TArray<FEnemyWaveData> SpawnWaves;
};
