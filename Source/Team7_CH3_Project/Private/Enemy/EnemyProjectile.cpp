// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"

AEnemyProjectile::AEnemyProjectile()
{
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCheck"));
    CollisionComponent->InitSphereRadius(15.0f);
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileCheck"));
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
}


