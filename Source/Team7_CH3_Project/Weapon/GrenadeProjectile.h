// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GrenadeProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class TEAM7_CH3_PROJECT_API AGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenadeProjectile();

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UParticleSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> ExplosionCameraShake;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	TObjectPtr<class UMaterialInterface> ExplosionDecal;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	float FlashIntensity = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FLinearColor FlashColor;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// 충돌 시 실행될 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 실제 폭발 로직
	void Explode();

public:	
	// WeaponComponent에서 스탯을 전달받을 변수
	float MaxDamage = 0.f;
	float MinDamage = 0.f; 
	float InnerRadius = 0.f; 
	float ExplosionRadius = 0.f;
	float ImpulseStrength = 0.f;

};
