#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackSlotWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UAttackSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    class UImage* Image_AttackIcon;
    UPROPERTY(meta = (BindWidget))
    class UImage* Image_AttackFrame;
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar_AttackGauge;

    // 쿨타임 계산 변수
    float MaxCooldownTime = 0.0f;
    float CurrentRemainingTime = 0.0f;
    bool bIsCoolingDown = false;

public:
    
    UPROPERTY(EditAnywhere, Category = "Settings")
    UTexture2D* SlotIconTexture; // 에디터 디테일창에서 개별적으로 아이콘 추가

    virtual void NativeConstruct() override;

    bool IsReady() const { return !bIsCoolingDown; }

    void SetSlotIcon(UTexture2D* NewIcon); // 초기 이미지 설정
    void SetFeedbackColor(FLinearColor NewColor); // 클릭 시 피드백 컬러 변경
    void StartDelay(float DelayTime); // 쿨타임(딜레이) 시작 함수

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override; // 매 프레임 프로그레스바 갱신
};
