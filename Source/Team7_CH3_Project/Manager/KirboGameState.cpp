// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameState.h"
#include "KirboGameInstance.h"
#include "Stages/StageSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"

void AKirboGameState::BeginPlay()
{
    Super::BeginPlay();

    UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance());
    if (GI)
    {
        CurrentStageIndex = GI->CurrentStageIndex;
        if (StageDataTable)
        {
            FString RowName = FString::FromInt(CurrentStageIndex);
            CurrentStageData = StageDataTable->FindRow<FStageData>(FName(*RowName), "");

            if (CurrentStageData)
            {
                FTimerHandle StageStartHandle;
                GetWorldTimerManager().SetTimer(StageStartHandle, this, &AKirboGameState::StartStage, CurrentStageData->StageStartDelay, false);
            }
        }
    }
}

void AKirboGameState::UpdateScoreAndKills(int ScoreAmount)
{
	CurrentScore += ScoreAmount;
	CurrentKills++;

    if (UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance()))
    {
        GI->AddScore(ScoreAmount);
        GI->AddKills(1);
    }

    if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        UISub->CurrentScore = this->CurrentScore;
        UISub->BroadcastHUDUpdate(CurrentStageIndex, CurrentWaveIndex + 1, CurrentKills, CurrentScore);
    }
}

void AKirboGameState::StartStage()
{
    CurrentWaveIndex = 0;
    NextWave();
}
void AKirboGameState::NextWave()
{
    if (CurrentStageData && LevelSpawner && CurrentStageData->SpawnWaves.IsValidIndex(CurrentWaveIndex))
    {
        LevelSpawner->SpawnWave(CurrentStageData->SpawnWaves[CurrentWaveIndex]);
    }
}
void AKirboGameState::OnWaveCleared()
{
    CurrentWaveIndex++;
    if (CurrentStageData && CurrentWaveIndex < CurrentStageData->SpawnWaves.Num())
    {
        FTimerHandle WaveDelayHandle;
        GetWorldTimerManager().SetTimer(WaveDelayHandle, this, &AKirboGameState::NextWave, CurrentStageData->WavesDelay, false);
    }
    else
    {
        FTimerHandle StageDelayHandle;
        GetWorldTimerManager().SetTimer(StageDelayHandle, this, &AKirboGameState::OnStageCleared, CurrentStageData->StageEndDelay, false);
    }
}
void AKirboGameState::OnStageCleared()
{
    if (UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance()))
    {
        GI->MoveToNextStage();
    }
}
