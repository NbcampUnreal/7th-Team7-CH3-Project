#include "DevHMainMenuWidget.h"
#include "Kismet/GameplayStatics.h" // 레벨 이동
#include "Kismet/KismetSystemLibrary.h" // 게임 종료


void UDevHMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton) // 1. 시작 버튼 연결
    {
        StartButton->OnMyClicked.AddDynamic(this, &UDevHMainMenuWidget::OnStartGame);
    }

    if (ExitButton) // 2. 나가기 버튼 연결
    {
        ExitButton->OnMyClicked.AddDynamic(this, &UDevHMainMenuWidget::OnExitGame);
    }
}

void UDevHMainMenuWidget::OnStartGame()
{
    if (LevelToLoad != NAME_None) // LevelToLoad에 할당된 레벨 열기
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
    }
}

void UDevHMainMenuWidget::OnExitGame()
{
    // 게임 프로그램 종료
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
