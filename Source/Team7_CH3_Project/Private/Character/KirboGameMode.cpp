// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboGameMode.h"
#include "Character/KirboCharacter.h"
#include "Character/KirboPlayerController.h"
#include "Team7_CH3_Project/Manager/KirboGameState.h"
#include "Team7_CH3_Project/Manager/KirboGameInstance.h"

AKirboGameMode::AKirboGameMode()
{
	DefaultPawnClass = AKirboCharacter::StaticClass();
	PlayerControllerClass = AKirboPlayerController::StaticClass();
}

void AKirboGameMode::OnEnemyKilled(int ScoreDrop)
{
	if (AKirboGameState* KGS = GetGameState<AKirboGameState>())
	{
		KGS->UpdateScoreAndKills(ScoreDrop);
	}
}

void AKirboGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance()))
	{
		GI->InitializeBGMAndUI(GetWorld());
	}
}
