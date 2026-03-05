// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KirboPlayerController.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AKirboPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AKirboPlayerController();

protected:
	virtual void BeginPlay() override;

    virtual void PlayerTick(float DeltaTime) override; // KH 260223 추가 : 크로스헤어 구현
    void UpdateCrosshair(); // KH 260223 추가 : 크로스헤어 구현

    bool bIsGameOver = false; // 게임 종료 여부를 저장하는 변수

public:
    // 게임 종료 시 호출하여 커서와 입력을 UI로 넘기는 함수
    void SwitchToResultUIInput(class UUserWidget* FocusWidget);



protected:
    virtual void SetupInputComponent() override;

    // 키 입력 시 실행될 실제 함수들
    void VolumeUp();
    void VolumeDown();
};
