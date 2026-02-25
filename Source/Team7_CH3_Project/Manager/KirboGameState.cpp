// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameState.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"

void AKirboGameState::UpdateScoreAndKills(int ScoreAmount)
{
	CurrentStageScore += ScoreAmount;
	CurrentKills++;

	if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
	{
		UISub->CurrentScore = this->CurrentStageScore;
		UISub->BroadcastHUDUpdate(1, CurrentWave, CurrentKills, CurrentStageScore);
	}
}
