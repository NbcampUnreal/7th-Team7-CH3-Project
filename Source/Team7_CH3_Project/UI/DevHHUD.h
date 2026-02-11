#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DevHHUD.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API ADevHHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "UI | Classes")
    TSubclassOf<class UDevHGameResultWidget> ResultWidgetClass; // WBP_GameResult 지정
    UPROPERTY(EditAnywhere, Category = "UI | Classes")
    TSubclassOf<class UUserWidget> MainHUDClass; // WBP_HUD 지정

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleGameResult(bool bIsClear); // 실제 결과 UI 띄우는 내부 함수

private:
    UPROPERTY()
    UUserWidget* MainHUDInstance; // HUD 인스턴스
    UPROPERTY()
    class UDevHGameResultWidget* ResultWidgetInstance; // 생성된 결과 위젯 저장, 중복 생성 방지
};
