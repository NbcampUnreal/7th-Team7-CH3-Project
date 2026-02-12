#include "DevHMainMenuPlayerController.h"
#include "Blueprint/UserWidget.h" // CreateWidget


void ADevHMainMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();


    if (MainMenuWidgetClass)
    {
        // 1. 위젯 생성
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport(); // 2. 위젯 화면에 붙임

            bShowMouseCursor = true; // 마우스 커서 visible
            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(false); // 마우스 커서 안사라지게
            SetInputMode(InputMode); // 입력 모드를 UI만 가능하도록 설정
        }
    }
}
