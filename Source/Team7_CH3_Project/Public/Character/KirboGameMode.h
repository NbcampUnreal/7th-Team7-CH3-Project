// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Blueprint/UserWidget.h" // 위젯 사용을 위해 필요합니다.
#include "KirboGameMode.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AKirboGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AKirboGameMode();

	void OnEnemyKilled(int ScoreDrop);

protected:
    virtual void BeginPlay() override;

public:
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kirbo|Audio")
    class USoundBase* MainMenuMusicAsset; // 메인 메뉴 BGM
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kirbo|Audio")
    class USoundBase* InGameMusicAsset; // 게임 플레이 BGM
    UPROPERTY()
    class UAudioComponent* BGMComponent; // 현재 재생 중인 사운드를 관리할 컴포넌트

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kirbo|UI")
    TSubclassOf<UUserWidget> VolumeWidgetClass; // 에디터에서 볼륨 표시 위젯 할당
    UPROPERTY(BlueprintReadOnly, Category = "Kirbo|UI")
    class UUserWidget* VolumeWidgetInstance;

    void PlayBackgroundMusic(USoundBase* NewMusic); // 사운드 재생 함수

public:
   
    UFUNCTION(BlueprintCallable, Category = "Kirbo|Audio")
    void ChangeVolume(float Delta); // 볼륨을 일정 수치만큼 변경 함수

    UFUNCTION(BlueprintImplementableEvent, Category = "Kirbo|UI")
    void OnVolumeChangedUI(float NewVolume); // 위젯 내부에 볼륨 수치를 전달하는 함수

private:
    float CurrentVolume = 1.0f; // 현재 볼륨 수치 (0.0 ~ 1.0 사이로 유지)
};
