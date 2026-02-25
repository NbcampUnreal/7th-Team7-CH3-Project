#include "DevHHUD.h"
#include "DevHUISubSystem.h"
#include "DevHGameResultWidget.h"
#include "Blueprint/UserWidget.h"
#include "Team7_CH3_Project/UI/GameLogTypes.h"
#include "Team7_CH3_Project/Public/Character/KirboPlayerController.h"


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
            // 게임 결과 신호
            UISub->OnGameResultChanged.AddDynamic(this, &ADevHHUD::HandleGameResult);
        }
    }

    // 커서 생성 및 관리
    if (CrosshairWidgetClass)
    {
        CrosshairInstance = CreateWidget<UDevHCrosshairWidget>(GetOwningPlayerController(), CrosshairWidgetClass);
        if (CrosshairInstance)
        {
            CrosshairInstance->AddToViewport(10);
            UE_LOG(LogTemp, Log, TEXT("Crosshair Widget Created Successfully!"));
        }
    }
}


void ADevHHUD::HandleGameResult(bool bIsClear)
{
    // 기존 크로스헤어 위젯 숨기기 또는 제거
    if (CrosshairInstance)
    {
        CrosshairInstance->RemoveFromParent();
    }

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

    // 결과창 뜨면 컨트롤러의 입력 모드를 전환
    if (AKirboPlayerController* PC = Cast<AKirboPlayerController>(GetOwningPlayerController()))
    {
        PC->SwitchToResultUIInput(ResultWidgetInstance);
    }
}

void ADevHHUD::AddGameLog(FName RowName)
{
    // 데이터 테이블 할당 확인
    if (!LogDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("LogDataTable is NOT assigned in HUD!"));
        return;
    }

    // 데이터 테이블에서 RowName에 해당하는 행 찾기
    static const FString ContextString(TEXT("LogDataLookup"));
    FLogData* FoundRow = LogDataTable->FindRow<FLogData>(RowName, ContextString);

    if (FoundRow)
    {
        // 찾은 데이터를 블루프린트 이벤트 OnReceiveLog로 전달
        OnReceiveLog(*FoundRow);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find Log Row: %s"), *RowName.ToString());
    }
}
