#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterWaveRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSpawnInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SkeletalMeshName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FMonsterWaveRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 WaveNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMonsterSpawnInfo> Monsters;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PreWaveDelay = 2.f;
};
