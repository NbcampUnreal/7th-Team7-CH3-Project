// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/ICombatEntity.h"
#include "EnemyData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FEnemyData() :
        HealthMax(100.0f),
        Defence(5.0f),
        Damage(10.0f),
        AttackRange(150.0f),
        AttackCooldown(2.0f),
        Movespeed(300.0f),
        RangeProjectileSpeed(1500.f),
        WaveHealthInc(0.05f),
        StageHealthInc(0.2f),
        WaveDamageInc(0.03f),
        StageDamageInc(0.15f)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HealthMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    EAttackType AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Movespeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Melee")
    float MeleeAttackAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Ranged")
    float RangeProjectileSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    float WaveHealthInc;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    float StageHealthInc;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    float WaveDamageInc;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
    float StageDamageInc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object")
    TSubclassOf<class AEnemyProjectile> ProjectileObj;
};
