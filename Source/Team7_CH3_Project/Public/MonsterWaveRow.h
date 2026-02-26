#pragma once
//MonsterWaveRow.h
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterWaveRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSpawnInfo
{
    GENERATED_BODY()

    // 이미 만들어진 Enemy BP 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class ABaseEnemy> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StageID = 1;  // 1, 2, 3 등
};

USTRUCT(BlueprintType)
struct FMonsterWaveRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WaveNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMonsterSpawnInfo> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PreWaveDelay = 2.f;
};
