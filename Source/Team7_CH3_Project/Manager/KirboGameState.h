// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "KirboGameState.generated.h"

class UKirboGameInstance;
class UDevHUISubSystem;
class AStageSpawner;
struct FStageData;

UCLASS()
class TEAM7_CH3_PROJECT_API AKirboGameState : public AGameState
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

	UDataTable* StageDataTable;
	AStageSpawner* LevelSpawner;

	UKirboGameInstance* GI;
	UDevHUISubSystem* UISystem;
	FStageData* CurrentStageData;

	void UpdateUI();
	void StartStage();
	void NextWave();
	void OnWaveCleared();
	void OnStageCleared();
	void ActivateStagePortal();
	void BroadcastLog(FName LogRowName);

	UFUNCTION(BlueprintCallable, Category = "Stage")
	void EnterStagePortal();
};
