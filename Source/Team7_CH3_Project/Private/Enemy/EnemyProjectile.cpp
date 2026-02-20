// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KirboCharacter.h"

AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    CollisionComponent->InitSphereRadius(10.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
    ProjectileMovement->InitialSpeed = 2500.f;
    ProjectileMovement->MaxSpeed = 2500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
}

void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
}

void AEnemyProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float DistanceTraveled = FVector::Dist(SpawnLocation, GetActorLocation());
    if (DistanceTraveled > MaxRange)
    {
        // Any effects for destroying projectile? or not maybe.
        Destroy();
    }
}

void AEnemyProjectile::InitializeProjectile(float speed, float damage, float maxRange)
{
    ProjectileMovement->InitialSpeed = speed;
    ProjectileMovement->MaxSpeed = speed;
    DamageValue = damage;
    MaxRange = maxRange;

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileOverlap);
    CollisionComponent->SetNotifyRigidBodyCollision(true);
}

void AEnemyProjectile::OnProjectileOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        AKirboCharacter* CheckPlayer = Cast<AKirboCharacter>(OtherActor);
        if (CheckPlayer)
        {
            UGameplayStatics::ApplyDamage(
                OtherActor,
                DamageValue,
                GetInstigatorController(),
                this,
                UDamageType::StaticClass()
            );

            Destroy();
        }
    }
}
