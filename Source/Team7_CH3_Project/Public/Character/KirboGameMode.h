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
};
