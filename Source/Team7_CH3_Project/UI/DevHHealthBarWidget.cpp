#include "DevHHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "DevHUISubSystem.h"
#include "Components/TextBlock.h"
#include "Team7_CH3_Project/Public/Character/KirboStatComponent.h"
#include "Kismet/KismetMathLibrary.h" // FMath 사용을 위해 확인


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

    // 안전 장치 : 생성 직후 현재 플레이어의 데이터를 직접 가져와 초기화
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            // 캐릭터에서 스탯 컴포넌트를 찾아 직접 값을 가져옴
            if (UKirboStatComponent* Stat = Pawn->FindComponentByClass<UKirboStatComponent>())
            {
                float InitialPercent = (Stat->BaseStat.MaxHP > 0) ? (Stat->CurrentHP / Stat->BaseStat.MaxHP) : 1.0f;
                CurrentPercent = InitialPercent;
                TargetPercent = InitialPercent;

                if (HealthBar)
                {
                    HealthBar->SetPercent(CurrentPercent);
                }

                UpdateHealthBar(Stat->CurrentHP, Stat->BaseStat.MaxHP);
            }
        }
    }
}

void UDevHHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 현재 퍼센트가 목표 퍼센트와 다르다면 보간 진행
    if (!FMath::IsNearlyEqual(CurrentPercent, TargetPercent, 0.0001f))
    {
        // FInterpTo : 현재값에서 목표값으로 DeltaTime에 맞춰 부드럽게 이동
        CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, InterpSpeed);

        // 계산된 현재 퍼센트를 프로그래스 바에 적용
        if (HealthBar)
        {
            HealthBar->SetPercent(CurrentPercent);
        }
    }
}

void UDevHHealthBarWidget::UpdateHealthBar(float CurrentHP, float MaxHP)
{
    // 프로그래스 바 업데이트
    if (HealthBar && MaxHP > 0.0f)
    {
        TargetPercent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
    }

    // 텍스트 블록 업데이트
    if (TextBlock_Health && !HealthFormat.IsEmpty())
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("CurrentHP"), FMath::RoundToInt(CurrentHP)); // FMath::RoundToInt -> 소수점 정수 변환해줌
        Args.Add(TEXT("MaxHP"), FMath::RoundToInt(MaxHP));

        TextBlock_Health->SetText(FText::Format(HealthFormat, Args));
    }
}
