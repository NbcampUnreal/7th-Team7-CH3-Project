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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TSubclassOf<class AActor> BulletObj;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    class UNiagaraSystem* WallHitVFX;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    class UNiagaraSystem* PlayerHitVFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    class USoundBase* HitSound;
};
