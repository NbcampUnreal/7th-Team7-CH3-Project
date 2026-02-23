// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Particles/ParticleSystem.h"
#include "GrenadeStat.generated.h"

USTRUCT(BlueprintType)
struct FGrenadeStat : public FTableRowBase
{
	GENERATED_BODY()

	// 물리 및 투척 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LaunchForce = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GravityScale = 0.2f;

	// 폭발 및 데미지 로직
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius = 300.f; // 전체 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float MaxDamage = 150.f; // 중심부 최대 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float MinDamage = 30.f; // 가장자리 최소 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float InnerRadius = 150.f; // 100% 데미지가 들어가는 핵심 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ImpulseStrength = 5000.f; // 주변 사물을 밀어내는 힘의 세기

	// 시스템 및 밸런스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	float CooldownTime = 1.0f; // 다시 쏠 수 있을 때까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	float RegenTime = 30.0f; // 수류탄이 다시 생기는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	int32 MaxCharges = 3; // 최대 보유 가능 개수

	// 시각/청각 피드백
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class UParticleSystem> MuzzleFlash; // 발사 시 어깨에서 터지는 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class UParticleSystem> ExplosionEffect; // 폭발 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class USoundBase> LaunchSound; // 발사 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class USoundBase> ExplosionSound; // 폭발 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class UMaterialInterface> ExplosionDecal; // 폭발 지면에 남길 그을림 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float FlashIntensity = 5000.f; // 폭발 순간 반짝이는 빛의 세기 및 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FLinearColor FlashColor = FLinearColor(1.0f, 0.6f, 0.2f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> FireCameraShake; // 발사 시 화면 흔들림
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> ExplosionCameraShake;
	

	//  클래스 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class AActor> GrenadeClass;
};
