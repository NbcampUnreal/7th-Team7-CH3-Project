// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()

public:
    AEnemyProjectile();

protected:
    float BurstRadius;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, Category = "DamageView")
    float DamageValue;

    void Explode(AActor* HitActor);

private:
    FVector SpawnLocation;
    float MaxRange;
    bool isHitted = false;

    UParticleSystem* EffectHitGround;
    float EffectHitGroundSize;
    UParticleSystem* EffectHitPlayer;
    float EffectHitPlayerSize;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    void InitializeProjectile(float Speed, float Damage, float Range, float GravityScale = 0.0f, float AOERadius = 0.0f);
    void InitializeEffects(UParticleSystem* HitGround, float HitGroundSize, UParticleSystem* HitPlayer, float HitPlayerSize);
};
