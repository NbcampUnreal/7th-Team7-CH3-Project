#pragma once
//MonsterSpawner.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h" // ✅ UDataTable 사용
#include "MonsterSpawner.generated.h"

// Forward Declaration
class UWaveManager;

UCLASS()
class TEAM7_CH3_PROJECT_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:

    static int32 CurrentStage;

    AMonsterSpawner();
    virtual void BeginPlay() override;

    /** NavMesh 기반 랜덤 위치 반환 */
    bool GetRandomNavMeshLocation(FVector& OutLocation);

    /** Debug: 현재 스테이지 반환 */
    int32 GetCurrentStage() const { return StageNumber; }

    /** Debug: 현재 웨이브 반환 */
    int32 GetCurrentWave() const;

    UPROPERTY(EditAnywhere, Category = "Wave")
    UDataTable* WaveDataTable;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 StageNumber;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float SpawnRadius = 500.f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, Category = "Wave")
    AActor* LinkedNavMesh;

private:
    UPROPERTY()
    UWaveManager* WaveManager;
};
