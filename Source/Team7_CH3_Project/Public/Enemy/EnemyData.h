// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "IEnemy.h"
#include "EnemyData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MonsterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAttackType AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealthMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Defense;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Movespeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldDrop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ItemDropChance;
};
