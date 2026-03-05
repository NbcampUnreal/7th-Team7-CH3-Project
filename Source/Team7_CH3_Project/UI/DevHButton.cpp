#include "DevHButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDevHButton::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (InternalText)
    {
        // 에디터에서 설정한 텍스트 -> 실제 텍스트 블록에 적용
        InternalText->SetText(ButtonDisplayName);
    }
}

void UDevHButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (InternalButton)
    {
        // 내부 버튼의 클릭 이벤트와 함수 연결
        InternalButton->OnClicked.AddDynamic(this, &UDevHButton::HandleInternalClick);
    }
}

void UDevHButton::HandleInternalClick()
{
    if (OnMyClicked.IsBound())
    {
        // 눌렀다고 바인딩한 부모 위젯들에게 방송
        OnMyClicked.Broadcast();
    }
}
