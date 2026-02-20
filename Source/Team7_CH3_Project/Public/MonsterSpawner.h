#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MonsterWaveRow.h"
#include "SimpleMonster.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMonsterSpawner();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "Wave")
    UDataTable* WaveDataTable;

    UPROPERTY(EditAnywhere)
    float SpawnRadius = 2000.f;

    UPROPERTY(EditAnywhere)
    FVector SpawnCenter;

private:
    int32 CurrentWave = 1;
    int32 AliveMonsterCount = 0;

    void StartWave(int32 WaveNumber);
    void StartNextWave();

    void SpawnSingleMonster(const FString& MeshName);
    bool GetRandomNavMeshLocation(FVector& OutLocation);

    UFUNCTION()
    void OnMonsterDead(ASimpleMonster* DeadMonster);
};
