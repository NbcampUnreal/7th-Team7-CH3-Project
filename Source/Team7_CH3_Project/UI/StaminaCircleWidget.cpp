#include "StaminaCircleWidget.h"
#include "Components/Image.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"
#include "Team7_CH3_Project/Public/Character/KirboStatComponent.h"


void UStaminaCircleWidget::NativeConstruct()
{
    if (StaminaBarImage)
    {
        StaminaMID = StaminaBarImage->GetDynamicMaterial();
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            UISub->OnStaminaChanged.AddDynamic(this, &UStaminaCircleWidget::UpdateStamina);

            // 생성 직후 이미 캐릭터에 저장된 값을 한 번 업데이트
            if (APawn* Pawn = GetOwningPlayerPawn())
            {
                if (UActorComponent* FoundComp = Pawn->GetComponentByClass(UKirboStatComponent::StaticClass()))
                {
                    if (UKirboStatComponent* StatComp = Cast<UKirboStatComponent>(FoundComp))
                    {
                        // 초기값 수동 갱신
                        UpdateStamina(StatComp->CurrentStamina, StatComp->BaseStat.MaxStamina);
                    }
                }
            }
        }
    }
}

void UStaminaCircleWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
    if (StaminaMID && MaxStamina > 0.0f)
    {
        float Percent = CurrentStamina / MaxStamina;
        // 마테리얼의 Percent 파라미터 갱신
        StaminaMID->SetScalarParameterValue(TEXT("Percent"), Percent);
    }
}
