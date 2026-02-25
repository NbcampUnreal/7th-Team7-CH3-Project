//MonsterSpawner.cpp
#include "MonsterSpawner.h"
#include "WaveManager.h" // cpp에서만 include
#include "NavigationSystem.h"
#include "Engine/World.h"

int32 AMonsterSpawner::CurrentStage = 1;

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();


    //  현재 스테이지와 다르면 이 스포너는 작동 안함
    if (StageNumber != CurrentStage)
    {
        return;
    }

    // WaveManager 생성 및 초기화
    WaveManager = NewObject<UWaveManager>(this); // ✅ Outer 지정
    WaveManager->Initialize(this, WaveDataTable, StageNumber);

    if (bAutoStart)
    {
        WaveManager->StartWave(1);
    }
}

bool AMonsterSpawner::GetRandomNavMeshLocation(FVector& OutLocation)
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return false;

    FNavLocation NavLocation;
    FVector Center = LinkedNavMesh ? LinkedNavMesh->GetActorLocation() : GetActorLocation();

    if (NavSys->GetRandomPointInNavigableRadius(Center, SpawnRadius, NavLocation))
    {
        OutLocation = NavLocation.Location;
        return true;
    }

    return false;
}

int32 AMonsterSpawner::GetCurrentWave() const
{
    return WaveManager ? WaveManager->GetCurrentWaveNumber() : 0;
}
