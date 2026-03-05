// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyObjectData.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UEnemyObjectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    class UNiagaraSystem* HitVFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    class USoundBase* AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    class USoundBase* HitSound;
};
