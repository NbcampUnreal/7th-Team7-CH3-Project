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

    CollisionComponent->SetGenerateOverlapEvents(true);
    CollisionComponent->SetNotifyRigidBodyCollision(true);

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnProjectileHit);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileOverlap);

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

void AEnemyProjectile::InitializeProjectile(float Speed, float Damage, float Range, float GravityScale, float AOERadius)
{
    DamageValue = Damage;
    MaxRange = Range;
    BurstRadius = AOERadius;

    if (ProjectileMovement)
    {
        ProjectileMovement->InitialSpeed = Speed;
        ProjectileMovement->MaxSpeed = Speed;
        ProjectileMovement->ProjectileGravityScale = GravityScale;

        ProjectileMovement->Velocity = GetActorRotation().Vector() * Speed;
        ProjectileMovement->UpdateComponentVelocity();
    }
}

void AEnemyProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;

    Explode(OtherActor);
}

void AEnemyProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;
    if (OtherActor->ActorHasTag("Enemy")) return;

    Explode(OtherActor);
}

void AEnemyProjectile::Explode(AActor* HitActor)
{
    FVector ExplosionOrigin = GetActorLocation();

    ExplosionOrigin.Z += 20.0f;

    if (BurstRadius > 1.0f)
    {
        UGameplayStatics::ApplyRadialDamage(
            this, DamageValue, ExplosionOrigin, BurstRadius,
            UDamageType::StaticClass(), { this, GetInstigator() },
            this, GetInstigatorController(), true
        );
        DrawDebugSphere(GetWorld(), ExplosionOrigin, BurstRadius, 12, FColor::Red, false, 2.0f);
    }

    AKirboCharacter* Player = Cast<AKirboCharacter>(HitActor);
    if (Player)
    {
        UGameplayStatics::ApplyDamage(Player, DamageValue, GetInstigatorController(), this, UDamageType::StaticClass());
    }

    Destroy();
}
