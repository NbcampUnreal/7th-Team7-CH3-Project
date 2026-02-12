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

public:
    AEnemyProjectile();

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    void Launch(FVector Direction);

};
