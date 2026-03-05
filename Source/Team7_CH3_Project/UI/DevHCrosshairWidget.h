#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DevHCrosshairWidget.generated.h"


UCLASS()
class TEAM7_CH3_PROJECT_API UDevHCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* CrosshairImage; // 조준점 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* HitMarkerImage; // 히트마커 이미지

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    class UWidgetAnimation* HitMarkerAnim; // 히트마커 애니메이션 변수

public:
    // 호버 상태에 따라 색상을 변경하는 함수
    void SetTargetingColor(bool bIsEnemy);
    // 히트마커 애니메이션을 실행하는 함수
    void PlayHitMarker();
};
