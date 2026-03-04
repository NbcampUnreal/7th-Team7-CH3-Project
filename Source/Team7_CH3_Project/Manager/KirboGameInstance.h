// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h" // 위젯
#include "KirboGameInstance.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UKirboGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData")
	UDataTable* StageDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* MainMenuMusicAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* InGameMusicAsset;
	UPROPERTY()
	class UAudioComponent* BGMComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ResultWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VolumeWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* VolumeWidgetInstance;

	int32 CurrentStageIndex = 0;
	int32 TotalScore = 0;
	int32 TotalKills = 0;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void MoveToNextStage(FName SceneName);

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void GameClear();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void GameOver();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AddScore(int32 AddedScores);
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AddKills(int32 AddedKills);
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void SetScore(int32 SetScores);
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void SetKills(int32 SetKills);

	UFUNCTION(BlueprintCallable)
	int32 GetTotalScore() const { return TotalScore; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayBackgroundMusic(USoundBase* NewMusic, UWorld* InWorld);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ChangeVolume(float Delta);
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnVolumeChangedUI(float NewVolume);
	UFUNCTION(BlueprintCallable, Category = "System")
	void InitializeBGMAndUI(UWorld* InWorld);

private:
	float CurrentVolume = 1.0f;
};
