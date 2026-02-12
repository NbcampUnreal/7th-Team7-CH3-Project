#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHButton.h"
#include "DevHMainMenuWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // WBP_MainMenu에 배치한 버튼 위젯
    UPROPERTY(meta = (BlindWidget))
    UDevHButton* StartButton;
    UPROPERTY(meta = (BlindWidget))
    UDevHButton* ExitButton;
    
    // 이동할 레벨 이름 에디터에서 생성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Config")
    FName LevelToLoad = FName("L_TopDownTest"); /*레벨 교체 시 수정*/

protected:
    virtual void NativeConstruct() override; // 초기화
    
    UFUNCTION()
    void OnStartGame(); // 시작 버튼 클릭한 경우
    UFUNCTION()
    void OnExitGame(); // 나가기 버튼 클릭한 경우
};
