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
	UKirboGameInstance();

	UPROPERTY(BlueprintReadOnly, Category = "StageData")
	int32 CurrentStageIndex = 0;
	UPROPERTY(BlueprintReadOnly, Category = "StageData")
	TArray<FName> SceneList;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int32 TotalScore = 0;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int32 TotalKills = 0;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void MoveToNextStage();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AddScore(int32 AddedScores);
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AddKills(int32 AddedKills);

	float GetStageIndex() { return CurrentStageIndex; }
	float GetScore() { return TotalScore; }
	float GetKills() { return TotalKills; }
};
