// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()

private:
    FVector SpawnLocation;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    float DamageValue;
    float MaxRange;
    AActor* EnemyFired;

    AEnemyProjectile();

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    void InitializeProjectile(float speed, float damage, float maxRange);

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
