// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameInstance.h"
#include "Kismet/GameplayStatics.h"

UKirboGameInstance::UKirboGameInstance()
{
    CurrentStageIndex = 0;
    TotalScore = 0;
    TotalKills = 0;
}

void UKirboGameInstance::MoveToNextStage()
{
    CurrentStageIndex++;

    if (SceneList.IsValidIndex(CurrentStageIndex))
    {
        UGameplayStatics::OpenLevel(GetWorld(), SceneList[CurrentStageIndex]);
    }
    else
    {
        // Algorithm about game finished mechanism - or call it from other script
    }
}

void UKirboGameInstance::AddScore(int32 AddedScores)
{
    TotalScore += AddedScores;
}
void UKirboGameInstance::AddKills(int32 AddedKills)
{
    TotalKills += AddedKills;
}
