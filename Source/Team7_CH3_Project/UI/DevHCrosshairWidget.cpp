#include "DevHCrosshairWidget.h"

void UDevHCrosshairWidget::SetTargetingColor(bool bIsEnemy)
{
    if (CrosshairImage)
    {
        // 적이면 빨간색, 아니면 흰색
        FLinearColor NewColor = bIsEnemy ? FLinearColor::Red : FLinearColor::White;
        CrosshairImage->SetColorAndOpacity(NewColor);
    }
}
