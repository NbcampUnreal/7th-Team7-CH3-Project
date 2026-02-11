#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DevHMainMenuPlayerController.generated.h"

UCLASS()
class TEAMPROJECTSAMPLE_API ADevHMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass; // 에디터에서 WBP 선택 가능

    UPROPERTY()
    class UUserWidget* MainMenuWidgetInstance; // 위젯 인스턴스 보관 변수
};
