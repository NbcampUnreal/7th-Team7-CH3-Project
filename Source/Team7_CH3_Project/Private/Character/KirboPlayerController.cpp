// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboPlayerController.h"
#include "Team7_CH3_Project/UI/DevHHUD.h"
#include "Engine/EngineTypes.h" // KH 260223 추가 : 크로스헤어 구현
#include "Math/Vector2D.h"      // KH 260223 추가 : 크로스헤어 구현

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

    // KH 260223 추가 : 윈도우 기본 커서 모양 없앰
    CurrentMouseCursor = EMouseCursor::None;
}

// KH 260223 추가 : 크로스헤어 구현
void AKirboPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    UpdateCrosshair();
}

// KH 260223 추가 : 크로스헤어 구현
void AKirboPlayerController::UpdateCrosshair()
{
    ADevHHUD* HUDPtr = Cast<ADevHHUD>(GetHUD());
    if (!HUDPtr || !HUDPtr->GetCrosshairWidget()) return;

    FVector2D MousePosition;
    // GetMousePosition은 bool을 반환 -> 실제 좌표는 MousePosition 변수에 담김
    if (GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        // 레이캐스트를 쏠 때 이미 구한 MousePosition을 그대로 넣음
        FHitResult HitResult;
        bool bHit = GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, false, HitResult);
        bool bIsEnemy = (bHit && HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(FName("Enemy")));

        // 위젯 업데이트
        HUDPtr->GetCrosshairWidget()->SetTargetingColor(bIsEnemy);
        HUDPtr->GetCrosshairWidget()->SetPositionInViewport(MousePosition);
    }
}
