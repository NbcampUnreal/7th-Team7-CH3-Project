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
};
