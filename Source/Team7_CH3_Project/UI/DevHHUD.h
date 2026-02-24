#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Team7_CH3_Project/UI/DevHCrosshairWidget.h"
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
    UPROPERTY(EditAnywhere, Category = "UI | Classes")
    TSubclassOf<class UDevHCrosshairWidget> CrosshairWidgetClass; // WBP_Cursor 지정을 위한 변수 추가


protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleGameResult(bool bIsClear); // 실제 결과 UI 띄우는 내부 함수

private:
    UPROPERTY(BlueprintReadOnly, Category = "UI | Instances", meta = (AllowPrivateAccess = "true"))
    UUserWidget* MainHUDInstance; // HUD 인스턴스

    UPROPERTY(BlueprintReadOnly, Category = "UI | Instances", meta = (AllowPrivateAccess = "true"))
    class UDevHGameResultWidget* ResultWidgetInstance; // 생성된 결과 위젯 저장, 중복 생성 방지

public:
    // 외부(PlayerController)에서 조준점에 접근하기 위한 함수
    UDevHCrosshairWidget* GetCrosshairWidget() const { return CrosshairInstance; }

private:
    UPROPERTY()
    UDevHCrosshairWidget* CrosshairInstance;


// 로그 UI
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Log")
    UDataTable* LogDataTable; // 데이터 테이블 에셋 에디터에 할당
   
    UFUNCTION(BlueprintImplementableEvent, Category = "Log")
    void OnReceiveLog(const FLogData& Data); // WBP_Log에서 이 이벤트를 구현하여 로그를 화면에 표시

    UFUNCTION(BlueprintCallable, Category = "Log")
    void AddGameLog(FName RowName); // 외부에서 호출할 함수
};
