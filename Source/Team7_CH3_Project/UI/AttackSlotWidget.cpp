#include "AttackSlotWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void UAttackSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 부모 설정 전 에디터에서 직접 넣은 기본값이 있다면 적용
    if (Image_AttackIcon && SlotIconTexture)
    {
        Image_AttackIcon->SetBrushFromTexture(SlotIconTexture);
    }
    // 초기 상태 설정
    if (ProgressBar_AttackGauge)
    {
        ProgressBar_AttackGauge->SetPercent(0.0f);
    }
}

void UAttackSlotWidget::SetSlotIcon(UTexture2D* NewIcon)
{
    if (Image_AttackIcon && NewIcon)
    {
        // 슬롯의 아이콘 이미지 변경
        Image_AttackIcon->SetBrushFromTexture(NewIcon);
    }
}

void UAttackSlotWidget::SetFeedbackColor(FLinearColor NewColor)
{
    if (Image_AttackFrame)
        Image_AttackFrame->SetColorAndOpacity(NewColor);
}

void UAttackSlotWidget::StartDelay(float DelayTime)
{
    if (DelayTime <= 0.f) return;

    MaxCooldownTime = DelayTime;
    CurrentRemainingTime = DelayTime;
    bIsCoolingDown = true;

    if (ProgressBar_AttackGauge)
        ProgressBar_AttackGauge->SetPercent(1.0f);
}

void UAttackSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bIsCoolingDown) return;

    CurrentRemainingTime -= InDeltaTime;
    float Percent = FMath::Clamp(CurrentRemainingTime / MaxCooldownTime, 0.0f, 1.0f);

    if (ProgressBar_AttackGauge)
        ProgressBar_AttackGauge->SetPercent(Percent);

    if (CurrentRemainingTime <= 0.0f)
    {
        bIsCoolingDown = false;
        SetFeedbackColor(FLinearColor::White);
    }
}
