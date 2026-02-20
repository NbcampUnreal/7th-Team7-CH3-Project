#include "MonsterSpawner.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (SpawnCenter.IsZero())
        SpawnCenter = GetActorLocation();

    StartWave(CurrentWave);
}

void AMonsterSpawner::StartWave(int32 WaveNumber)
{
    if (!WaveDataTable) return;

    static const FString Context(TEXT("WaveContext"));

    FMonsterWaveRow* Row =
        WaveDataTable->FindRow<FMonsterWaveRow>(
            FName(*FString::Printf(TEXT("Wave%d"), WaveNumber)),
            Context);

    if (!Row) return;

    AliveMonsterCount = 0;

    for (const FMonsterSpawnInfo& Info : Row->Monsters)
    {
        for (int32 i = 0; i < Info.Count; i++)
        {
            SpawnSingleMonster(Info.SkeletalMeshName);
        }
    }
}

void AMonsterSpawner::SpawnSingleMonster(const FString& MeshName)
{
    FVector SpawnLocation;
    if (!GetRandomNavMeshLocation(SpawnLocation))
        return;

    ASimpleMonster* Monster =
        GetWorld()->SpawnActor<ASimpleMonster>(
            ASimpleMonster::StaticClass(),
            SpawnLocation,
            FRotator::ZeroRotator);

    if (Monster)
    {
        Monster->SetMeshByName(MeshName);

        AliveMonsterCount++;

        Monster->OnMonsterDeath.AddDynamic(
            this,
            &AMonsterSpawner::OnMonsterDead);
    }
}

void AMonsterSpawner::OnMonsterDead(ASimpleMonster* DeadMonster)
{
    AliveMonsterCount--;

    if (AliveMonsterCount <= 0)
    {
        StartNextWave();
    }
}

void AMonsterSpawner::StartNextWave()
{
    CurrentWave++;

    FTimerHandle Timer;

    GetWorld()->GetTimerManager().SetTimer(
        Timer,
        [this]()
        {
            StartWave(CurrentWave);
        },
        3.0f,
        false);
}

bool AMonsterSpawner::GetRandomNavMeshLocation(FVector& OutLocation)
{
    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    if (!NavSys) return false;

    FNavLocation NavLocation;

    if (NavSys->GetRandomPointInNavigableRadius(
        SpawnCenter,
        SpawnRadius,
        NavLocation))
    {
        OutLocation = NavLocation.Location;
        return true;
    }

    return false;
}
