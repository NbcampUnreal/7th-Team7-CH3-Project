#include "DevHMainMenuWidget.h"
#include "Kismet/GameplayStatics.h" // 레벨 이동
#include "Kismet/KismetSystemLibrary.h" // 게임 종료
#include "Team7_CH3_Project/Manager/KirboGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Team7_CH3_Project/UI/DevInfoWidget.h"
#include "GameFramework/PlayerController.h"


void UDevHMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton) // 시작 버튼 연결
    {
        StartButton->OnMyClicked.AddDynamic(this, &UDevHMainMenuWidget::OnStartGame);
    }

    if (ExitButton) // 나가기 버튼 연결
    {
        ExitButton->OnMyClicked.AddDynamic(this, &UDevHMainMenuWidget::OnExitGame);
    }

    if (DevInfoButton) // 개발자 정보 버튼 연결
    {
        DevInfoButton->OnMyClicked.AddDynamic(this, &UDevHMainMenuWidget::OnShowDevInfo);
    }
}

void UDevHMainMenuWidget::OnStartGame()
{
    // 현재 게임 GameInstance를 가져와서 캐스팅
    UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance());

    if (GI)
    {
        GI->StartGame();
    }
    else
    {
        // 캐스팅 실패 시 대비 (테스트용 로그)
        UE_LOG(LogTemp, Error, TEXT("GameInstance casting failed!"));

        if (LevelToLoad != NAME_None) // LevelToLoad에 할당된 레벨 열기
        {
            UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
        }
    }
}

void UDevHMainMenuWidget::OnExitGame()
{
    // 게임 프로그램 종료
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UDevHMainMenuWidget::OnShowDevInfo()
{
    // 중복 생성 방지
    if (CachedDevInfoWidget && CachedDevInfoWidget->IsInViewport())
    {
        return;
    }

    if (DevInfoWidgetClass)
    {
        CachedDevInfoWidget = CreateWidget<UUserWidget>(GetWorld(), DevInfoWidgetClass);

        if (CachedDevInfoWidget)
        {
            CachedDevInfoWidget->AddToViewport();

            if (MainMenuGroup)
            {
                MainMenuGroup->SetVisibility(ESlateVisibility::Collapsed);
                UDevInfoWidget* DevInfo = Cast<UDevInfoWidget>(CachedDevInfoWidget);
                if (DevInfo)
                {
                    DevInfo->OnClosed.AddDynamic(this, &UDevHMainMenuWidget::OnRestoreMainMenu);
                }
            }

            APlayerController* PC = GetOwningPlayer();
            if (PC)
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(CachedDevInfoWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DevInfoWidgetClass is NOT assigned in WBP_MainMenu!"));
    }
}

void UDevHMainMenuWidget::OnRestoreMainMenu()
{
    if (MainMenuGroup)
    {
        MainMenuGroup->SetVisibility(ESlateVisibility::Visible);
    }
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(this->TakeWidget());
        PC->SetInputMode(InputMode);
    }
}
