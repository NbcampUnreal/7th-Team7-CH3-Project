#include "DevHStageWave.h"
#include "DevHUISubSystem.h"
#include "Team7_CH3_Project/Manager/KirboGameState.h"

void UDevHStageWave::NativeConstruct()
{
    Super::NativeConstruct();

    // 텍스트 블록 포멧들 변수에 백업
    if (StageWaveText) StageWaveFormat = StageWaveText->GetText();
    if (KillsCountText) KillsCountFormat = KillsCountText->GetText();
    if (ScoreText) ScoreFormat = ScoreText->GetText();

    if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        // 데이터 변할 때마다 UpdateHUD 호출
        UISub->OnHUDDataChanged.AddDynamic(this, &UDevHStageWave::UpdateHUD);

        // 초기 UI 값 설정
        if (AKirboGameState* GS = GetWorld()->GetGameState<AKirboGameState>())
        {
            // 첫 킬 전에도 데이터 올바르게 표시
            UpdateHUD(GS->CurrentStageIndex, GS->CurrentWaveIndex, GS->CurrentKills, GS->CurrentScore);
        }
    }

}

void UDevHStageWave::UpdateHUD(int32 Stage, int32 Wave, int32 Kills, int32 Score)
{
    if (StageWaveText && !StageWaveFormat.IsEmpty()) // 1. Stage & Wave 업데이트
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("Stage"), Stage); // {Stage} 부분 데이터 매칭
        Args.Add(TEXT("Wave"), Wave); // {Wave} 부분 데이터 매칭

        // 에디터에서 "Stage{Stage} : Wave{Wave}" 형식을 가져와 값을 채움
        StageWaveText->SetText(FText::Format(StageWaveFormat, Args));
    }

    if (KillsCountText && !KillsCountFormat.IsEmpty()) // 2. Kills Count 업데이트
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("Kills"), Kills);

        KillsCountText->SetText(FText::Format(KillsCountFormat, Args));
    }

    if (ScoreText && !ScoreFormat.IsEmpty()) // 3. Score 업데이트
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("Score"), Score);

        ScoreText->SetText(FText::Format(ScoreFormat, Args));
    }
}
