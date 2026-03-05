//MonsterSpawnerDebug.cpp
#include "MonsterSpawnerDebug.h"
#include "MonsterSpawner.h"
#include "Kismet/GameplayStatics.h" // ✅ 추가
#include "Engine/Engine.h"

void AMonsterSpawnerDebug::BeginPlay()
{
    Super::BeginPlay();

    PrintWaveDebug();
}

void AMonsterSpawnerDebug::PrintWaveDebug()
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundSpawners);

    for (AActor* Actor : FoundSpawners)
    {
        if (AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(Actor))
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    5.f,
                    FColor::Cyan,
                    FString::Printf(TEXT("Current Stage: %d, Wave: %d"), Spawner->GetCurrentStage(), Spawner->GetCurrentWave())
                );
            }
        }
    }
}
