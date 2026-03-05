#include "EnemyHealthBarWidget.h"

void UEnemyHealthBarWidget::UpdateHealthBar(float Percent)
{
    // 목표 퍼센트 설정
    TargetPercent = FMath::Clamp(Percent, 0.0f, 1.0f);
}

void UEnemyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 현재 값과 목표 값이 다를 때만 계산 진행
    if (!FMath::IsNearlyEqual(CurrentPercent, TargetPercent, 0.001f))
    {
        // FInterpTo를 사용하여 CurrentPercent를 TargetPercent로 부드럽게 이동
        CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, InterpSpeed);

        if (EnemyHealthBar)
        {
            EnemyHealthBar->SetPercent(CurrentPercent);
        }
    }
}
