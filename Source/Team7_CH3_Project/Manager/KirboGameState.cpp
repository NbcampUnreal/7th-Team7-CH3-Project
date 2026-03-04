// Fill out your copyright notice in the Description page of Project Settings.


#include "KirboGameState.h"
#include "KirboGameInstance.h"
#include "StageDoor.h"
#include "Stages/StageData.h"
#include "Stages/StageSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"
#include "Team7_CH3_Project/UI/DevHHUD.h"

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

	// 레벨 진입 로그 - HUD 생성 시간을 고려해 0.5초 뒤 한 번만 실행
	FTimerHandle LogHandle;
	GetWorldTimerManager().SetTimer(LogHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (CurrentStageIndex == 0)
				BroadcastLog(FName("Game_Start"));
			BroadcastLog(FName("Stage_Next"));
		}), 0.5f, false);
}


void AKirboGameState::UpdateScoreAndKills(int ScoreAmount)
{
	CurrentScore += ScoreAmount;
	CurrentKills++;

	BroadcastLog(FName("Kill_Enemy")); // 처치 로그 즉시 호출

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

		BroadcastLog(FName("Wave_Start")); // 웨이브 시작 로그 추가
		LevelSpawner->SpawnWave(CurrentWaveIndex, CurrentStageData->SpawnWaves[CurrentWaveIndex]);
	}
}
void AKirboGameState::OnWaveCleared()
{
	BroadcastLog(FName("Wave_End")); // 웨이브 종료 로그 출력

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
		GI->SetScore(CurrentScore);
		GI->SetKills(CurrentKills);

		if (CurrentStageIndex == 0)
		{
			ActivateStagePortal();
			return;
		}

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

	BroadcastLog(FName("Stage_End"));
}

void AKirboGameState::ActivateStagePortal()
{
	TArray<AActor*> FoundPortals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageDoor::StaticClass(), FoundPortals);

	for (AActor* Actor : FoundPortals)
	{
		if (AStageDoor* Portal = Cast<AStageDoor>(Actor))
		{
			Portal->OpenDoor();
		}
	}

}
void AKirboGameState::BroadcastLog(FName LogRowName)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ADevHHUD* HUD = Cast<ADevHHUD>(PC->GetHUD()))
		{
			HUD->AddGameLog(LogRowName);
		}
	}
}
