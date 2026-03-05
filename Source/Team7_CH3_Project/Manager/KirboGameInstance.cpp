// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameInstance.h"
#include "Stages/StageData.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"

void UKirboGameInstance::StartGame()
{
	CurrentStageIndex = 0;
	TotalScore = 0;
	TotalKills = 0;

	if (!StageDataTable) return;

	TArray<FStageData*> AllRows;
	StageDataTable->GetAllRows<FStageData>(TEXT("ContextString"), AllRows);
	if (AllRows.Num() > 0)
	{
		UGameplayStatics::OpenLevel(GetWorld(), AllRows[0]->LevelName);
		UE_LOG(LogTemp, Log, TEXT("-----Game started!-----"));
	}
}

void UKirboGameInstance::MoveToNextStage(FName SceneName)
{
	CurrentStageIndex++;
	UGameplayStatics::OpenLevel(GetWorld(), SceneName);
}

void UKirboGameInstance::GameClear()
{
	UE_LOG(LogTemp, Log, TEXT("-----All stage and wave finished! Here we can input clear functions from other class maybe?-----"));
	// Algorithm about game finished mechanism - or call it from other script

	// UI 서브시스템에 클리어 신호를 보냄
	if (UDevHUISubSystem* UISub = GetSubsystem<UDevHUISubSystem>())
	{
		UISub->OnGameResultChanged.Broadcast(true); // true = Clear
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 일시정지
}

void UKirboGameInstance::GameOver()
{
	// UI 서브시스템에 클리어 신호를 보냄
	if (UDevHUISubSystem* UISub = GetSubsystem<UDevHUISubSystem>())
	{
		UISub->OnGameResultChanged.Broadcast(false); // false = Fail
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 일시정지
}

void UKirboGameInstance::AddScore(int32 AddedScores)
{
	TotalScore += AddedScores;
}

void UKirboGameInstance::AddKills(int32 AddedKills)
{
	TotalKills += AddedKills;
}

void UKirboGameInstance::SetScore(int32 SetScores)
{
	TotalScore = SetScores;
}

void UKirboGameInstance::SetKills(int32 SetKills)
{
	TotalKills = SetKills;
}

void UKirboGameInstance::InitializeBGMAndUI(UWorld* InWorld)
{
	if (!InWorld) return;

	FString CurrentLevelName = InWorld->GetMapName();
	CurrentLevelName.RemoveFromStart(InWorld->StreamingLevelsPrefix);

	if (CurrentLevelName.Equals(TEXT("L_MainMenu")))
	{
		if (MainMenuMusicAsset) PlayBackgroundMusic(MainMenuMusicAsset, InWorld);
	}
	else
	{
		if (InGameMusicAsset) PlayBackgroundMusic(InGameMusicAsset, InWorld);
	}

	if (VolumeWidgetClass && !VolumeWidgetInstance)
	{
		VolumeWidgetInstance = CreateWidget<UUserWidget>(InWorld, VolumeWidgetClass);
		if (VolumeWidgetInstance)
		{
			VolumeWidgetInstance->AddToViewport();
			VolumeWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			OnVolumeChangedUI(CurrentVolume);
		}
	}
}

void UKirboGameInstance::PlayBackgroundMusic(USoundBase* NewMusic, UWorld* InWorld)
{
	if (!NewMusic || !InWorld) return;

	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(1.0f, 0.0f);
	}

	BGMComponent = UGameplayStatics::SpawnSound2D(InWorld, NewMusic);

	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(CurrentVolume);
		BGMComponent->Play();
	}
}

void UKirboGameInstance::ChangeVolume(float Delta)
{
	CurrentVolume = FMath::Clamp(CurrentVolume + Delta, 0.0f, 1.0f);

	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(CurrentVolume);
	}

	OnVolumeChangedUI(CurrentVolume);
}
