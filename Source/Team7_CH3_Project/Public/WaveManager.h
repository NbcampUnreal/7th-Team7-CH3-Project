//WaveManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimerManager.h"
#include "WaveManager.generated.h"

// Forward Declaration
class AMonsterSpawner;
class ABaseEnemy;
class UDataTable;

UCLASS()
class TEAM7_CH3_PROJECT_API UWaveManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(AMonsterSpawner* Spawner, UDataTable* InWaveDataTable, int32 InStageNumber);
    void StartWave(int32 WaveNumber);

    /** Debug: 현재 웨이브 반환 */
    int32 GetCurrentWaveNumber() const { return CurrentWave; }

private:
    void CheckWaveClear();
    void StartNextWave();
    void OpenStageDoors();

private:
    UPROPERTY()
    AMonsterSpawner* LinkedSpawner;

    UPROPERTY()
    UDataTable* WaveDataTable;

    int32 StageNumber;
    int32 CurrentWave;
    int32 MaxWavePerStage = 3;
    TArray<ABaseEnemy*> CurrentWaveEnemies;

    FTimerHandle WaveCheckTimer;
};
