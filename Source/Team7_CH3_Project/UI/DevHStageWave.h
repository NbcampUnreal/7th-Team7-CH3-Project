#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" // 텍스트 블록
#include "DevHStageWave.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHStageWave : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StageWaveText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillsCountText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreText;

    // 원본 포맷 보관
    FText StageWaveFormat;
    FText KillsCountFormat;
    FText ScoreFormat;

public:
    virtual void NativeConstruct() override;
    // 데이터 받아 UI 갱신 -> 값 변할 때 외부에서 해당 함수 호출!
    UFUNCTION() // AddDynamic 바인딩하려면 UFUNCTION() 필수
    void UpdateHUD(int32 Stage, int32 Wave, int32 Kills, int32 Score);
};
