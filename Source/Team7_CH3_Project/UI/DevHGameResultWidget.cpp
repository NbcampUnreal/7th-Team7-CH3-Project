#include "DevHGameResultWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"
#include "Team7_CH3_Project/Manager/KirboGameState.h"


void UDevHGameResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RestartButton) // 시작 버튼 연결
    {
        RestartButton->OnMyClicked.AddDynamic(this, &UDevHGameResultWidget::OnRestartGame);
    }

    if (ExitButton) // 나가기 버튼 연결
    {
        ExitButton->OnMyClicked.AddDynamic(this, &UDevHGameResultWidget::OnExitGame);
    }
}

void UDevHGameResultWidget::OnRestartGame()
{
    if (LevelToLoad != NAME_None) // LevelToLoad에 할당된 레벨 열기
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
    }
}

void UDevHGameResultWidget::OnExitGame()
{
    // 게임 프로그램 종료
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UDevHGameResultWidget::SetupResultUI(bool bIsClear)
{
    if (GameResultText)
    {
        if (bIsClear) // 게임 클리어 시 -> 설정 메세지, 컬러 적용
        {
            GameResultText->SetText(GameClearMessage);
            GameResultText->SetColorAndOpacity(FSlateColor(GameClearColor));

            RestartButton->SetVisibility(ESlateVisibility::Hidden); // 재시작 버튼 숨김
        }
        else // 게임 오버 시 -> 설정 메세지, 컬러 적용
        {
            GameResultText->SetText(GameOverMessage);
            GameResultText->SetColorAndOpacity(FSlateColor(GameOverColor));

            RestartButton->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (AKirboGameState* GS = GetWorld()->GetGameState<AKirboGameState>())
    {
        // 점수 텍스트 업데이트
        if (ScoreText)
        {
            ScoreText->SetText(FText::AsNumber(GS->CurrentScore));
        }

        if (KillCountText)
        {
            KillCountText->SetText(FText::AsNumber(GS->CurrentKills));
        }
    }

    // 등장 애니메이션 재생
    if (IntroAnim)
    {
        PlayAnimation(IntroAnim);
    }
}
