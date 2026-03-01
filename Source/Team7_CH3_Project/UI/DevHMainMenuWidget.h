#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHButton.h"
#include "MediaPlayer.h"
#include "FileMediaSource.h"
#include "DevHMainMenuWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // WBP_MainMenu에 배치한 버튼 위젯
    UPROPERTY(meta = (BindWidget))
    UDevHButton* StartButton;
    UPROPERTY(meta = (BindWidget))
    UDevHButton* ExitButton;
    UPROPERTY(meta = (BindWidget))
    UDevHButton* DevInfoButton;
    UPROPERTY(meta = (BindWidget))
    class UWidget* MainMenuGroup;
    
    // 이동할 레벨 이름 에디터에서 생성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Config")
    FName LevelToLoad = FName("L_TopDownTest"); /*레벨 교체 시 수정*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Config")
    TSubclassOf<UUserWidget> DevInfoWidgetClass;

    UPROPERTY()
    UUserWidget* CachedDevInfoWidget; // 생성된 위젯 기억

    // 미디어
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    UMediaPlayer* MediaPlayer;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    UFileMediaSource* VideoSource;

protected:
    virtual void NativeConstruct() override; // 초기화
    
    UFUNCTION()
    void OnStartGame(); // 시작 버튼 클릭한 경우
    UFUNCTION()
    void OnExitGame(); // 나가기 버튼 클릭한 경우
    UFUNCTION()
    void OnShowDevInfo(); // 개발자 정보 버튼 클릭한 경우
    UFUNCTION()
    void OnRestoreMainMenu(); // 메인메뉴 UI 다시 띄우기
};
