#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MonsterSpawner.generated.h"

// 전방 선언: 컴파일 속도를 높이고 순환 참조를 방지합니다.
class UWaveManager;
class ABaseEnemy;

UCLASS()
class TEAM7_CH3_PROJECT_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    // 모든 스포너가 공유하는 현재 스테이지 번호
    static int32 CurrentStage;

    AMonsterSpawner();
    virtual void BeginPlay() override;

    /** NavMesh 기반 랜덤 위치 반환 */
    bool GetRandomNavMeshLocation(FVector& OutLocation);

    /** 특정 웨이브를 시작하는 로직 */
    void StartWave(int32 WaveNumber);

    /** 개별 적을 생성하는 로직 */
    void SpawnSingleEnemy(TSubclassOf<ABaseEnemy> EnemyClass);

    /** 적이 제거되었을 때 호출될 델리게이트 함수 */
    UFUNCTION()
    void HandleEnemyDestroyed(AActor* DestroyedActor);

    /** 다음 웨이브로 넘어가기 위한 타이머 로직 */
    void StartNextWave();

    /** 현재 스테이지 반환 */
    int32 GetCurrentStage() const { return StageNumber; }
    /** 현재 웨이브 반환 */
    int32 GetCurrentWave() const;

    // --- 에디터 설정 변수 ---
    UPROPERTY(EditAnywhere, Category = "Wave")
    UDataTable* WaveDataTable;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 StageNumber = 1;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float SpawnRadius = 500.f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, Category = "Wave")
    AActor* LinkedNavMesh;

private:
    UPROPERTY()
    UWaveManager* WaveManager;

    // --- 내부 관리용 멤버 변수 ---
    FVector SpawnCenter;        // 스폰 중심 좌표
    int32 CurrentWave = 1;      // 현재 웨이브 번호
    int32 AliveEnemyCount = 0;   // 현재 필드에 살아있는 적의 수
};
