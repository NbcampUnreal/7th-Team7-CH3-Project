#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMonsterSpawner();

protected:
    virtual void BeginPlay() override;

    FTimerHandle SpawnTimerHandle;

    void SpawnMonster();

    // Get a random NavMesh location outside the camera view
    bool GetRandomNavMeshLocationOutsideCamera(FVector& OutLocation, float Radius, int MaxAttempts = 20);

    // Check if a point is visible on the player's TopDown camera
    bool IsPointInCameraViewTopDown(APlayerController* PC, FVector Point);

public:
    // BP_Monster or C++ monster class
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<AActor> MonsterClass;

    // Center of spawning area
    UPROPERTY(EditAnywhere, Category = "Spawning")
    FVector SpawnCenter;

    // How far from center to spawn
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnRadius;

    // Time interval between spawns
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval;

    // Debug visualization
    UPROPERTY(EditAnywhere, Category = "Spawning")
    bool bDrawDebug;
};
