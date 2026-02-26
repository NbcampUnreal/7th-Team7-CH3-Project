#include "MonsterSpawner.h"
#include "WaveManager.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
// 인클루드 경로는 Public 폴더 기준으로 작성하는 것이 표준입니다.
#include "Enemy/BaseEnemy.h"
#include "MonsterWaveRow.h"

int32 AMonsterSpawner::CurrentStage = 1;

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    // 1. 현재 월드의 스테이지와 이 스포너의 스테이지가 다르면 작동하지 않도록 함
    if (StageNumber != CurrentStage) return;

    // 2. 스폰 중심점 설정 (LinkedNavMesh가 있으면 그 위치, 없으면 스포너 본인 위치)
    SpawnCenter = LinkedNavMesh ? LinkedNavMesh->GetActorLocation() : GetActorLocation();

    // 3. WaveManager 초기화 (기존에 GetRandomNavMeshLocation에 잘못 들어가 있던 로직을 여기로 이동)
    WaveManager = NewObject<UWaveManager>(this);
    if (WaveManager)
    {
        WaveManager->Initialize(this, WaveDataTable, StageNumber);
    }

    // 4. 자동 시작 옵션이 있다면 첫 웨이브 실행
    if (bAutoStart)
    {
        StartWave(CurrentWave);
    }
}

void AMonsterSpawner::StartWave(int32 WaveNumber)
{
    if (!WaveDataTable) return;

    static const FString Context(TEXT("WaveContext"));

    // 데이터 테이블에서 해당 웨이브 행(Row) 정보 찾기
    FMonsterWaveRow* Row = WaveDataTable->FindRow<FMonsterWaveRow>(
        FName(*FString::Printf(TEXT("Wave%d"), WaveNumber)),
        Context);

    if (!Row) return;

    AliveEnemyCount = 0;

    // 행에 정의된 적 종류와 수만큼 생성 루프 실행
    for (const FMonsterSpawnInfo& Info : Row->Enemies)
    {
        for (int32 i = 0; i < Info.Count; i++)
        {
            SpawnSingleEnemy(Info.EnemyClass);
        }
    }
}

void AMonsterSpawner::SpawnSingleEnemy(TSubclassOf<ABaseEnemy> EnemyClass)
{
    if (!EnemyClass) return;

    FVector SpawnLocation;
    // NavMesh 위에서 유효한 위치를 찾지 못하면 생성 생략
    if (!GetRandomNavMeshLocation(SpawnLocation)) return;

    // 액터 스폰 (Collision 설정 추가로 겹침 문제 예방)
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABaseEnemy* Enemy = GetWorld()->SpawnActor<ABaseEnemy>(
        EnemyClass,
        SpawnLocation,
        FRotator::ZeroRotator,
        SpawnParams);

    if (Enemy)
    {
        AliveEnemyCount++;

        // 하민님 코드: 데이터 로드 (첫 번째 인자는 스테이지, 두 번째는 웨이브)
        Enemy->LoadData(StageNumber, CurrentWave);

        // 적이 죽었을 때를 감지하기 위해 OnDestroyed 델리게이트 바인딩
        Enemy->OnDestroyed.AddDynamic(this, &AMonsterSpawner::HandleEnemyDestroyed);
    }
}

void AMonsterSpawner::HandleEnemyDestroyed(AActor* DestroyedActor)
{
    AliveEnemyCount--;

    // 필드의 모든 적이 죽었을 때만 다음 웨이브 진행
    if (AliveEnemyCount <= 0)
    {
        StartNextWave();
    }
}

void AMonsterSpawner::StartNextWave()
{
    CurrentWave++;

    FTimerHandle Timer;
    // 3초 대기 후 다음 웨이브 시작 (람다 캡처 사용)
    GetWorld()->GetTimerManager().SetTimer(
        Timer,
        [this]()
        {
            this->StartWave(this->CurrentWave);
        },
        3.f,
        false);
}

// ✅ 중복되었던 함수를 하나로 통합하고 리턴값을 수정했습니다. [cite: 2026-01-14]
bool AMonsterSpawner::GetRandomNavMeshLocation(FVector& OutLocation)
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return false;

    FNavLocation NavLocation;
    // SpawnCenter(또는 LinkedNavMesh)를 기준으로 반지름 내의 랜덤 지점을 찾습니다.
    if (NavSys->GetRandomPointInNavigableRadius(SpawnCenter, SpawnRadius, NavLocation))
    {
        OutLocation = NavLocation.Location;
        return true;
    }

    return false;
}

int32 AMonsterSpawner::GetCurrentWave() const
{
    return WaveManager ? WaveManager->GetCurrentWaveNumber() : CurrentWave;
}
