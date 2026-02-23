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

void UDevHCrosshairWidget::PlayHitMarker()
{
    if (HitMarkerAnim && HitMarkerImage)
    {
        // 이미 재생 중이더라도 처음부터 다시 재생하여 연속 타격 피드백 줌
        PlayAnimation(HitMarkerAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
    }
}
