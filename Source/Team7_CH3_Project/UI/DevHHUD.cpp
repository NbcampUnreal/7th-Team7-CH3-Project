#include "DevHHUD.h"
#include "DevHUISubSystem.h"
#include "DevHGameResultWidget.h"
#include "Blueprint/UserWidget.h"

void ADevHHUD::BeginPlay()
{
    Super::BeginPlay();

    // HUD 생성 및 띄우기
    if (MainHUDClass)
    {
        MainHUDInstance = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
        if (MainHUDInstance)
        {
            MainHUDInstance->AddToViewport();
        }
    }

    // 게임 결과만 HUD에서 대기 (전역 데이터라)
    if (UGameInstance* GI = GetGameInstance()) // Subsystem 찾아 이벤트 바인딩
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 1. 게임 결과 신호
            UISub->OnGameResultChanged.AddDynamic(this, &ADevHHUD::HandleGameResult);
        }
    }
}


void ADevHHUD::HandleGameResult(bool bIsClear)
{
    if (ResultWidgetInstance) // 중복 생성 방지: 이미 있으면 내용만 업데이트
    {
        ResultWidgetInstance->SetupResultUI(bIsClear);
        return;
    }

    if (ResultWidgetClass) // 위젯 생성 및 출력
    {
        ResultWidgetInstance = CreateWidget<UDevHGameResultWidget>(
            GetOwningPlayerController(), ResultWidgetClass);
        if (ResultWidgetInstance)
        {
            ResultWidgetInstance->AddToViewport();
            ResultWidgetInstance->SetupResultUI(bIsClear);
        }
    }
}
