#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.h" // 기존 Spawner 포함
#include "MonsterSpawnerDebug.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AMonsterSpawnerDebug : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

private:
    void PrintWaveDebug();
};
