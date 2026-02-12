// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboPlayerController.h"

AKirboPlayerController::AKirboPlayerController()
{
	bShowMouseCursor = true;
}

void AKirboPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}
