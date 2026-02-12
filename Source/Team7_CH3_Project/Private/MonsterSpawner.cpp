#include "MonsterSpawner.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    SpawnCenter = FVector::ZeroVector;
    SpawnRadius = 1000.f;
    SpawnInterval = 5.f;
    bDrawDebug = true;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    // Use actor location as SpawnCenter if none specified
    if (SpawnCenter.IsZero())
        SpawnCenter = GetActorLocation();

    // Start repeating timer
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true);
}

// Spawn a monster on NavMesh outside camera view
void AMonsterSpawner::SpawnMonster()
{
    if (!MonsterClass) return;

    FVector SpawnLocation;
    if (GetRandomNavMeshLocationOutsideCamera(SpawnLocation, SpawnRadius, 20))
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

        if (bDrawDebug)
            DrawDebugSphere(GetWorld(), SpawnLocation, 50.f, 12, FColor::Green, false, 2.f);

        UE_LOG(LogTemp, Log, TEXT("Monster spawned at %s"), *SpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid NavMesh location outside camera view found"));
    }
}

// Get a random NavMesh location outside the TopDown camera view
bool AMonsterSpawner::GetRandomNavMeshLocationOutsideCamera(FVector& OutLocation, float Radius, int MaxAttempts)
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return false;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return false;

    for (int i = 0; i < MaxAttempts; i++)
    {
        FNavLocation NavLocation;
        if (NavSys->GetRandomPointInNavigableRadius(SpawnCenter, Radius, NavLocation))
        {
            if (!IsPointInCameraViewTopDown(PC, NavLocation.Location))
            {
                OutLocation = NavLocation.Location;
                return true;
            }
        }
    }

    return false; // Could not find valid location
}

// Check if a point is visible on the TopDown camera
bool AMonsterSpawner::IsPointInCameraViewTopDown(APlayerController* PC, FVector Point)
{
    if (!PC) return false;

    FVector2D ScreenPosition;
    bool bOnScreen = UGameplayStatics::ProjectWorldToScreen(PC, Point, ScreenPosition);

    if (!bOnScreen)
        return false;

    // Get viewport size
    int32 ViewportX, ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);

    // Check if point is inside the viewport rectangle
    return ScreenPosition.X >= 0 && ScreenPosition.X <= ViewportX &&
        ScreenPosition.Y >= 0 && ScreenPosition.Y <= ViewportY;
}
