// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KirboGameInstance.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UKirboGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	int32 TotalScore = 0;
};
