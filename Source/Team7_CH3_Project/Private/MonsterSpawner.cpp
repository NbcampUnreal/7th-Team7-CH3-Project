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

	AliveEnemyCount = 0;

	for (const FMonsterSpawnInfo& Info : Row->Enemies)
	{
		for (int32 i = 0; i < Info.Count; i++)
		{
			SpawnSingleEnemy(Info.EnemyClass);
		}
	}
}

void AMonsterSpawner::SpawnSingleEnemy(
	TSubclassOf<ABaseEnemy> EnemyClass)
{
	if (!EnemyClass) return;

	FVector SpawnLocation;
	if (!GetRandomNavMeshLocation(SpawnLocation))
		return;

	ABaseEnemy* Enemy =
		GetWorld()->SpawnActor<ABaseEnemy>(
			EnemyClass,
			SpawnLocation,
			FRotator::ZeroRotator);

	if (Enemy)
	{
		AliveEnemyCount++;

		//하민 : 이 코드 없으면 작동하지 않아서 임시로 추가했습니다
		Enemy->LoadData(1, CurrentWave);

		// 기존 Enemy 코드 수정 없이 전멸 감지
		Enemy->OnDestroyed.AddDynamic(
			this,
			&AMonsterSpawner::HandleEnemyDestroyed);
	}
}

void AMonsterSpawner::HandleEnemyDestroyed(
	AActor* DestroyedActor)
{
	AliveEnemyCount--;

	if (AliveEnemyCount <= 0)
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
		3.f,
		false);
}

bool AMonsterSpawner::GetRandomNavMeshLocation(
	FVector& OutLocation)
{
	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(
			GetWorld());

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
