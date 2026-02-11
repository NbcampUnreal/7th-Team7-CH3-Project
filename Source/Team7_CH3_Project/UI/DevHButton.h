#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHButton.generated.h"

// 델리게이트 선언: 버튼이 클릭되었을 때 부모에게 알리기 위한 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);


UCLASS()
class TEAMPROJECTSAMPLE_API UDevHButton : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Config")
    FText ButtonDisplayName; // 에디터에서 버튼 텍스트 수정 가능 변수
    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnButtonClicked OnMyClicked; // 부모 위젯(MainMenu)이 버튼 클릭 이벤트를 바인딩 가능

protected:
    UPROPERTY(meta = (BindWidget)) 
    class UButton* InternalButton; // WBP 버튼 위젯 이름과 똑같이
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* InternalText; // WBP 텍스트 블록 위젯 이름과 똑같이

protected:
    // 위젯이 화면 그려지기 전 실행 -> 텍스트 갱신
    virtual void NativePreConstruct() override;
    // 위젯이 게임에서 생성될 때 실행 -> 클릭 이벤트 연결
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleInternalClick(); // 내부 버튼 눌렀을 때 실행 함수


};
