// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboGameMode.h"
#include "Character/KirboCharacter.h"
#include "Character/KirboPlayerController.h"
#include "Team7_CH3_Project/Manager/KirboGameState.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AKirboGameMode::AKirboGameMode()
{
	DefaultPawnClass = AKirboCharacter::StaticClass();
	PlayerControllerClass = AKirboPlayerController::StaticClass();

    BGMComponent = nullptr;
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

    // 레벨 이름 확인 및 음악 재생 로직
    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    if (CurrentLevelName.Equals(TEXT("L_MainMenu")))
    {
        if (MainMenuMusicAsset) PlayBackgroundMusic(MainMenuMusicAsset);
    }
    else
    {
        if (InGameMusicAsset) PlayBackgroundMusic(InGameMusicAsset);
    }

    // 코드에서 직접 위젯 생성 및 출력
    if (VolumeWidgetClass)
    {
        VolumeWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), VolumeWidgetClass);
        if (VolumeWidgetInstance)
        {
            VolumeWidgetInstance->AddToViewport();
            // 처음 게임 시작 시에는 위젯 숨김
            VolumeWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

            OnVolumeChangedUI(CurrentVolume);
        }
    }
}

void AKirboGameMode::PlayBackgroundMusic(USoundBase* NewMusic)
{
    if (!NewMusic) return;

    // 이미 음악이 재생 중이라면 페이드 아웃 후 교체
    if (BGMComponent && BGMComponent->IsPlaying())
    {
        BGMComponent->FadeOut(1.0f, 0.0f);
    }

    // 새로운 배경 음악 재생
    BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), NewMusic);

    if (BGMComponent)
    {
        BGMComponent->Play();
    }
}

void AKirboGameMode::ChangeVolume(float Delta)
{
    // 볼륨을 0.0에서 1.0 사이로 유지
    CurrentVolume = FMath::Clamp(CurrentVolume + Delta, 0.0f, 1.0f);

    if (BGMComponent)
    {
        BGMComponent->SetVolumeMultiplier(CurrentVolume);
    }

    // UI 위젯에 변경된 볼륨 수치를 전달
    OnVolumeChangedUI(CurrentVolume);
}
