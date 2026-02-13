#include "AttackStatus.h"
#include "AttackSlotWidget.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"


void UAttackStatus::NativeConstruct()
{
    Super::NativeConstruct();

    // 서브시스템을 가져와서 델리게이트 바인딩
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 서브시스템에서 공격 신호가 오면 내 UI 함수 실행!
            UISub->OnAttackCooldownTriggered.AddDynamic(this, &UAttackStatus::TriggerNormalAttackUI);
            UISub->OnSkillCooldownTriggered.AddDynamic(this, &UAttackStatus::TriggerSkillUI);
        }
    }

    // 시작하자마자 지정된 이미지를 슬롯에 넣어줌
    if (WBP_AttackSlot_Normal && NormalAttackTexture)
        WBP_AttackSlot_Normal->SetSlotIcon(NormalAttackTexture);

    if (WBP_AttackSlot_Skill && SkillTexture)
        WBP_AttackSlot_Skill->SetSlotIcon(SkillTexture);
}

void UAttackStatus::TriggerNormalAttackUI(float Delay)
{
    if (WBP_AttackSlot_Normal)
    {
        WBP_AttackSlot_Normal->SetFeedbackColor(NormalHighlightColor);
        WBP_AttackSlot_Normal->StartDelay(Delay);
    }
}

void UAttackStatus::TriggerSkillUI(float Delay)
{
    if (WBP_AttackSlot_Skill)
    {
        WBP_AttackSlot_Skill->SetFeedbackColor(SkillHighlightColor);
        WBP_AttackSlot_Skill->StartDelay(Delay);
    }
}

void UAttackStatus::ResetFeedbackColor()
{
    // 기본 흰색(원래 컬러)으로 복구
    if (WBP_AttackSlot_Normal) WBP_AttackSlot_Normal->SetFeedbackColor(FLinearColor::White);
    if (WBP_AttackSlot_Skill) WBP_AttackSlot_Skill->SetFeedbackColor(FLinearColor::White);
}
