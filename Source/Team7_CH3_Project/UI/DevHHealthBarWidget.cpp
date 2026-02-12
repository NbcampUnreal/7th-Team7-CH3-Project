#include "DevHHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "DevHUISubSystem.h"
#include "Components/TextBlock.h"

void UDevHHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TextBlock_Health)
    {
        // 텍스트 가져와서 저장
        HealthFormat = TextBlock_Health->GetText();
    }

    // 서브시스템을 찾아서 UpdateHealthBar 직접 연결
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 서브시스템에서 HP 신호가 오면 자동으로 UpdateHealthBar 실행
            UISub->OnHPChanged.AddDynamic(this, &UDevHHealthBarWidget::UpdateHealthBar);
        }
    }
}

void UDevHHealthBarWidget::InitializeHealth(FName CharacterRowName)
{
    if (StatTable)
    {
        /* [데이터 연결 시점 가이드]
           1. 상단에 #include "다른사람이준파일.h" 추가
           2. 아래 주석 해제 
        */

        /*
        FCharacterData* Row = StatTable->FindRow<FCharacterData>(CharacterRowName, TEXT(""));
        if (Row)
        {
            // 구조체의 변수명이 MaxHP인지 확인 필수!
            CachedMaxHP = Row->MaxHP; 
            
            // 초기화 시점에 현재 체력(풀피)으로 업데이트
            UpdateHealthBar(CachedMaxHP, CachedMaxHP); 
        }
        */
    }
}

void UDevHHealthBarWidget::UpdateHealthBar(float CurrentHP, float MaxHP)
{
    // 1. 프로그래스 바 업데이트
    if (HealthBar && MaxHP > 0.0f)
    {
        float HealthPercent = CurrentHP / MaxHP;
        HealthBar->SetPercent(FMath::Clamp(HealthPercent, 0.0f, 1.0f)); // 0.0 ~ 1.0 사이로 값 고정!
    }

    // 2. 텍스트 블록 업데이트
    if (TextBlock_Health && !HealthFormat.IsEmpty())
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("CurrentHP"), FMath::RoundToInt(CurrentHP)); // FMath::RoundToInt -> 소수점 정수 변환해줌
        Args.Add(TEXT("MaxHP"), FMath::RoundToInt(MaxHP));

        TextBlock_Health->SetText(FText::Format(HealthFormat, Args));
    }
}