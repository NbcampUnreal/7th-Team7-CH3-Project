#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "EnemyHealthBarWidget.generated.h"


UCLASS()
class TEAM7_CH3_PROJECT_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateHealthBar(float Percent);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* EnemyHealthBar;
};
