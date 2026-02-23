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
};
