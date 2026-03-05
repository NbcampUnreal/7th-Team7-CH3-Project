#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHButton.h"
#include "DevInfoWidget.generated.h"


// 델리게이트 선언 - 닫힐 때 메인 메뉴에서 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDevInfoClosed);

UCLASS()
class TEAM7_CH3_PROJECT_API UDevInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UDevHButton* BackButton;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnClickBack(); // 뒤로가기

public:
    FOnDevInfoClosed OnClosed;
};
