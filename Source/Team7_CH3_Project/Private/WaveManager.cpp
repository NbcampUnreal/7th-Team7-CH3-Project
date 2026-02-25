#include "WaveManager.h"
#include "MonsterSpawner.h"
#include "MonsterWaveRow.h"
#include "Kismet/GameplayStatics.h"
#include "StageDoor.h"
#include "Enemy/BaseEnemy.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

void UWaveManager::Initialize(AMonsterSpawner* Spawner, UDataTable* InWaveDataTable, int32 InStageNumber)
{
    LinkedSpawner = Spawner;
    WaveDataTable = InWaveDataTable;
    StageNumber = InStageNumber;
    CurrentWave = 0;
    MaxWavePerStage = 3;
}

void UWaveManager::StartWave(int32 WaveNumber)
{
    if (!WaveDataTable || !LinkedSpawner) return;

    CurrentWave = WaveNumber;
    CurrentWaveEnemies.Empty();

    static const FString Context(TEXT("WaveContext"));

    FMonsterWaveRow* Row = WaveDataTable->FindRow<FMonsterWaveRow>(
        FName(*FString::Printf(TEXT("Wave%d"), WaveNumber)),
        Context);

    if (!Row) return;

    // Spawn all enemies
    for (const FMonsterSpawnInfo& Info : Row->Enemies)
    {
        for (int32 i = 0; i < Info.Count; i++)
        {
            FVector SpawnLocation;
            if (LinkedSpawner->GetRandomNavMeshLocation(SpawnLocation))
            {
                ABaseEnemy* Enemy = LinkedSpawner->GetWorld()->SpawnActor<ABaseEnemy>(
                    Info.EnemyClass,
                    SpawnLocation,
                    FRotator::ZeroRotator);

                if (Enemy)
                {
                    CurrentWaveEnemies.Add(Enemy);
                }
            }
        }
    }

    // Check wave clear periodically
    LinkedSpawner->GetWorld()->GetTimerManager().SetTimer(
        WaveCheckTimer,
        this,
        &UWaveManager::CheckWaveClear,
        0.5f,
        true);
}

void UWaveManager::CheckWaveClear()
{
    bool bAllDead = true;

    for (int32 i = CurrentWaveEnemies.Num() - 1; i >= 0; i--)
    {
        if (!IsValid(CurrentWaveEnemies[i]) || !CurrentWaveEnemies[i]->IsAlive())
        {
            CurrentWaveEnemies.RemoveAt(i);
            continue;
        }

        bAllDead = false;
    }

    if (bAllDead)
    {
        LinkedSpawner->GetWorld()->GetTimerManager().ClearTimer(WaveCheckTimer);

        // ✅ 웨이브 1~3 다 끝난 후 Stage Door 열기
        if (CurrentWave >= MaxWavePerStage) // MaxWavePerStage = 3
        {
            OpenStageDoors();
            StartNextWave();
        }
        else
        {
            // 다음 웨이브 시작
            StartNextWave();
        }
    }
}

void UWaveManager::StartNextWave()
{
    // ✅ Wave 3까지만 진행
    if (CurrentWave >= 3)
    {
        UE_LOG(LogTemp, Log, TEXT("Stage %d Completed (Max Wave Reached)"), StageNumber);
        return;
    }

    CurrentWave++;

    FMonsterWaveRow* Row = WaveDataTable->FindRow<FMonsterWaveRow>(
        FName(*FString::Printf(TEXT("Wave%d"), CurrentWave)),
        TEXT("NextWave"));

    if (Row)
    {
        LinkedSpawner->GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                StartWave(CurrentWave);
            });
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Stage %d Completed"), StageNumber);
    }
}

void UWaveManager::OpenStageDoors()
{
    TArray<AActor*> FoundDoors;
    UGameplayStatics::GetAllActorsOfClass(
        LinkedSpawner->GetWorld(),
        AStageDoor::StaticClass(),
        FoundDoors);

    for (AActor* Actor : FoundDoors)
    {
        if (AStageDoor* Door = Cast<AStageDoor>(Actor))
        {
            //  StageNumber 맞춰서 해당 스테이지 Door만 열기
            if (Door->StageNumber == LinkedSpawner->StageNumber)
            {
                Door->OpenDoor();
            }
        }
    }
}
