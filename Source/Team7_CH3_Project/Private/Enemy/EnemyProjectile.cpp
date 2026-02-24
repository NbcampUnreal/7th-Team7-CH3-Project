// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KirboCharacter.h"

AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(15.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1000.0f;
    ProjectileMovement->MaxSpeed = 1000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileOverlap);
    CollisionComponent->SetNotifyRigidBodyCollision(true);
}

void AEnemyProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FVector::Dist(SpawnLocation, GetActorLocation()) > MaxRange)
    {
        Destroy();
    }
}

void AEnemyProjectile::InitializeProjectile(float Speed, float Damage, float Range)
{
    DamageValue = Damage;
    MaxRange = Range;
    if (ProjectileMovement)
    {
        ProjectileMovement->InitialSpeed = Speed;
        ProjectileMovement->MaxSpeed = Speed;
        ProjectileMovement->Velocity = GetActorForwardVector() * Speed;
        ProjectileMovement->UpdateComponentVelocity();
    }
}

void AEnemyProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        AKirboCharacter* CheckPlayer = Cast<AKirboCharacter>(OtherActor);
        if (CheckPlayer)
        {
            UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, UDamageType::StaticClass());

            // Add VFX/Sound here

            Destroy();
        }
    }
}
