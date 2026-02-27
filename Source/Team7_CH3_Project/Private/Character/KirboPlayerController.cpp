// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboPlayerController.h"
#include "Team7_CH3_Project/UI/DevHHUD.h"
#include "Engine/EngineTypes.h" // KH 260223 추가 : 크로스헤어 구현
#include "Math/Vector2D.h"      // KH 260223 추가 : 크로스헤어 구현
#include "Character/KirboPlayerController.h"
#include "Character/KirboGameMode.h"
#include "Kismet/GameplayStatics.h"

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
    if (bIsGameOver) return; // 게임 오버 상태라면 더 이상 크로스헤어를 따라오게 하지 않음

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

void AKirboPlayerController::SwitchToResultUIInput(UUserWidget* FocusWidget)
{
    bIsGameOver = true; // 크로스헤어 업데이트 중단 플래그

    // 커서 모양을 다시 기본으로 복구
    CurrentMouseCursor = EMouseCursor::Default;
    bShowMouseCursor = true;

    // 입력 모드를 UI 전용으로 변경
    FInputModeUIOnly InputMode;
    if (FocusWidget)
    {
        // 결과창 위젯에 포커스를 주어 버튼 호버/클릭이 즉시 작동하게 함
        InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
    }
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    SetInputMode(InputMode);
}

void AKirboPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 키 바인딩 - 키 + 키 사용
    InputComponent->BindKey(EKeys::Equals, IE_Pressed, this, &AKirboPlayerController::VolumeUp);
    InputComponent->BindKey(EKeys::Hyphen, IE_Pressed, this, &AKirboPlayerController::VolumeDown);
}

void AKirboPlayerController::VolumeUp()
{
    // 현재 게임모드를 가져와서 볼륨 0.1 증가
    if (AKirboGameMode* GM = Cast<AKirboGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->ChangeVolume(0.1f);
    }
}

void AKirboPlayerController::VolumeDown()
{
    // 현재 게임모드를 가져와서 볼륨을 0.1 감소
    if (AKirboGameMode* GM = Cast<AKirboGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->ChangeVolume(-0.1f);
    }
}
