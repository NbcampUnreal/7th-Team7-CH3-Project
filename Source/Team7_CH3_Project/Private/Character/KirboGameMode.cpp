// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboGameMode.h"
#include "Character/KirboCharacter.h"
#include "Character/KirboPlayerController.h"

AKirboGameMode::AKirboGameMode()
{
	DefaultPawnClass = AKirboCharacter::StaticClass();
	PlayerControllerClass = AKirboPlayerController::StaticClass();
}
