#include "DamageFloatingText.h"
#include "Components/WidgetComponent.h"

ADamageFloatingText::ADamageFloatingText()
{
    PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	RootComponent = DamageWidgetComp;

	// 위젯이 월드 공간이 아닌 Screen 공간에 그려지도록 설정
	DamageWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
}

void ADamageFloatingText::BeginPlay()
{
	Super::BeginPlay();

    // 지정된 시간이 지나면 자동으로 월드에서 제거되도록 설정
    SetLifeSpan(LifeSpan);
}

void ADamageFloatingText::SetDamageValue(float DamageAmount)
{
    // C++에서 정의한 OnSetDamage 이벤트를 호출
    OnSetDamage(DamageAmount);
}
