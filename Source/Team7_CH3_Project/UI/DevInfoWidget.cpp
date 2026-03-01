#include "DevInfoWidget.h"
#include "Kismet/GameplayStatics.h"

void UDevInfoWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BackButton)
    {
        BackButton->OnMyClicked.AddDynamic(this, &UDevInfoWidget::OnClickBack);
    }
}

void UDevInfoWidget::OnClickBack()
{
    OnClosed.Broadcast();
    RemoveFromParent();

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FInputModeUIOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}
