// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameState.h"
#include "KirboGameInstance.h"
#include "Stages/StageData.h"
#include "Stages/StageSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"

void AKirboGameState::BeginPlay()
{
    Super::BeginPlay();

    GI = Cast<UKirboGameInstance>(GetGameInstance());
    if (GI)
    {
        CurrentStageIndex = GI->CurrentStageIndex;
        CurrentScore = GI->TotalScore;
        CurrentKills = GI->TotalKills;
        StageDataTable = GI->StageDataTable;
        if (StageDataTable)
        {
            TArray<FStageData*> AllRows;
            StageDataTable->GetAllRows<FStageData>(TEXT("ContextString"), AllRows);

            if (AllRows.IsValidIndex(CurrentStageIndex))
            {
                CurrentStageData = AllRows[CurrentStageIndex];
            }

            if (CurrentStageData)
            {
                FTimerHandle StageStartHandle;
                GetWorldTimerManager().SetTimer(StageStartHandle, this, &AKirboGameState::StartStage, CurrentStageData->StageStartDelay, false);
            }
        }
    }

    UISystem = GetGameInstance()->GetSubsystem<UDevHUISubSystem>();
    if (UISystem)
    {
        FTimerHandle UpdateUIHandle;
        GetWorldTimerManager().SetTimer(UpdateUIHandle, this, &AKirboGameState::UpdateUI, 0.25f, false);
    }
}


void AKirboGameState::UpdateScoreAndKills(int ScoreAmount)
{
	CurrentScore += ScoreAmount;
	CurrentKills++;

    if (UISystem)
    {
        UISystem->CurrentScore = this->CurrentScore;
        UISystem->BroadcastHUDUpdate(CurrentStageIndex + 1, CurrentWaveIndex + 1, CurrentKills, CurrentScore);
    }

    if (LevelSpawner)
    {
        LevelSpawner->OnEnemyDestroyed();
    }
}

void AKirboGameState::UpdateUI()
{
    UISystem->BroadcastHUDUpdate(CurrentStageIndex + 1, CurrentWaveIndex + 1, CurrentKills, CurrentScore);
}
void AKirboGameState::StartStage()
{
    CurrentWaveIndex = 0;
    UE_LOG(LogTemp, Log, TEXT("-----Wave started-----"));
    if (!LevelSpawner && GetWorld())
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

        FVector SpawnLocation = FVector::ZeroVector;
        FRotator SpawnRotation = FRotator::ZeroRotator;

        if (PlayerPawn)
        {
            SpawnLocation = PlayerPawn->GetActorLocation();
            SpawnRotation = PlayerPawn->GetActorRotation();
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Cast<APawn>(PlayerPawn);

        LevelSpawner = GetWorld()->SpawnActor<AStageSpawner>(AStageSpawner::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
        UE_LOG(LogTemp, Log, TEXT("-----New Spawner spawned on world!-----"));
    }

    LevelSpawner->InputStageIndex(CurrentStageIndex);
    NextWave();
}
void AKirboGameState::NextWave()
{
    if (CurrentStageData && LevelSpawner && CurrentStageData->SpawnWaves.IsValidIndex(CurrentWaveIndex))
    {
        LevelSpawner->SpawnWave(CurrentWaveIndex, CurrentStageData->SpawnWaves[CurrentWaveIndex]);
    }
}
void AKirboGameState::OnWaveCleared()
{
    CurrentWaveIndex++;
    if (CurrentStageData && CurrentWaveIndex < CurrentStageData->SpawnWaves.Num())
    {
        if (UISystem)
        {
            UISystem->CurrentScore = this->CurrentScore;
            UISystem->BroadcastHUDUpdate(CurrentStageIndex + 1, CurrentWaveIndex + 1, CurrentKills, CurrentScore);
        }
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
    if (GI)
    {
        GI->AddScore(CurrentScore);
        GI->AddKills(CurrentKills);

        TArray<FStageData*> AllRows;
        StageDataTable->GetAllRows<FStageData>(TEXT("ContextString"), AllRows);
        if (AllRows.Num() <= CurrentStageIndex + 1)
        {
            GI->GameClear();
        }
        else
        {
            GI->MoveToNextStage(AllRows[CurrentStageIndex + 1]->LevelName);
        }
    }

}
