#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHButton.h"
#include "DevHGameResultWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result | Text")
    FText GameClearMessage; // 클리어 메세지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result | Text")
    FText GameOverMessage; // 오버 메세지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result | Color")
    FLinearColor GameClearColor = FLinearColor::Blue; // 클리어 컬러
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result | Color")
    FLinearColor GameOverColor = FLinearColor::Red; // 오버 컬러


protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GameResultText; // WBP_GameResult에서 배치한 텍스트 블록

    UPROPERTY(meta = (BindWidget))
    UDevHButton* RestartButton;
    UPROPERTY(meta = (BindWidget))
    UDevHButton* ExitButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Config")
    FName LevelToLoad = FName("L_MainMenu"); /*레벨 교체 시 수정*/

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnRestartGame();
    UFUNCTION()
    void OnExitGame();

public:
    UFUNCTION(BlueprintCallable, Category = "Result")
    void SetupResultUI(bool bIsClear); // 게임 결과에 따른 UI 셋팅
};
