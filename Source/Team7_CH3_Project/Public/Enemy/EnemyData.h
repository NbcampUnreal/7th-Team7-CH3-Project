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
    // 기본 생성자: 여기서 모든 멤버 변수를 초기화합니다.
    FEnemyData()
        : MonsterName(TEXT("Default Name"))  // 문자열 초기화
        , AttackType(EAttackType::Melee)     // 열거형 초기화 (정의된 기본값 사용)
        , HealthMax(100.0f)                  // 모든 float 변수는 0.0f 혹은 기본값으로
        , Damage(10.0f)
        , Defence(0.0f)
        , AttackRange(150.0f)
        , AttackCooldown(2.0f)
        , Movespeed(300.0f)
        , MovespeedAct(300.f)
        , RangeProjectileSpeed(750.f)
        , GoldDrop(0)                        // int32 초기화
        , ItemDropChance(0.05f)              // 5% 확률 등 기본값 설정
        , WaveHealthInc(0.05f)
        , StageHealthInc(0.2f)
        , WaveDamageInc(0.03f)
        , StageDamageInc(0.15f)
    {
    }
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MonsterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAttackType AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealthMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Defence;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Movespeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovespeedAct;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RangeProjectileSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldDrop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ItemDropChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WaveHealthInc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StageHealthInc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WaveDamageInc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StageDamageInc;
};
