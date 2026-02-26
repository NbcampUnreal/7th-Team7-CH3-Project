// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Stages/StageData.h"
#include "KirboGameState.generated.h"

class AStageSpawner;

UCLASS()
class TEAM7_CH3_PROJECT_API AKirboGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	int32 CurrentStageIndex = 0;
	int32 CurrentWaveIndex = 0;

	int32 CurrentScore = 0;
	int32 CurrentKills = 0;

	void UpdateScoreAndKills(int ScoreAmount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	UDataTable* StageDataTable;
	UPROPERTY(BlueprintReadWrite, Category = "Spawning")
	AStageSpawner* LevelSpawner;

	FStageData* CurrentStageData;

	void StartStage();
	void NextWave();
	void OnWaveCleared();
	void OnStageCleared();
};
