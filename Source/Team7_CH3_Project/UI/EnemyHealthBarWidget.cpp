#include "EnemyHealthBarWidget.h"

void UEnemyHealthBarWidget::UpdateHealthBar(float Percent)
{
    // 프로그레스 바 컴포넌트가 유효한지 확인 후 값 설정
    if (EnemyHealthBar)
    {
        EnemyHealthBar->SetPercent(Percent);
    }
}
