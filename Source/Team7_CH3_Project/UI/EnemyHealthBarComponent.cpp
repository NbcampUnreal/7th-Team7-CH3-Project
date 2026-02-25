#include "EnemyHealthBarComponent.h"
#include "Team7_CH3_Project/UI/EnemyHealthBarWidget.h"


UEnemyHealthBarComponent::UEnemyHealthBarComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetWidgetSpace(EWidgetSpace::World); // 월드 공간에 UI 배치
    SetDrawAtDesiredSize(true); // 위젯 설정 크기대로 렌더링
}

void UEnemyHealthBarComponent::BeginPlay()
{
    Super::BeginPlay();
    SetVisibility(false); // 처음에는 숨겨둔 상태로 시작
}

void UEnemyHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 방어 코드 - 월드가 유효한지 확인
    UWorld* World = GetWorld();
    if (!World) return;

    // 방어 코드 2 - 플레이어 컨트롤러를 가져오고 IsValid 체크
    APlayerController* PC = World->GetFirstPlayerController();

    if (PC && PC->PlayerCameraManager && IsVisible())
    {
        APlayerCameraManager* CamManager = PC->PlayerCameraManager;

        FVector CameraLoc = CamManager->GetCameraLocation();
        FVector WidgetLoc = GetComponentLocation();

        FRotator NewRot = (CameraLoc - WidgetLoc).Rotation();

        // 계산된 회전값을 컴포넌트에 적용
        SetWorldRotation(NewRot);
    }
}

void UEnemyHealthBarComponent::ShowAndUpdateHP(float CurrentHP, float MaxHP)
{
    if (MaxHP <= 0.0f) return;

    SetVisibility(true); // 체력바 표시

    // 관리 중인 위젯을 가져와 체력 수치 갱신
    UEnemyHealthBarWidget* HPWidget = Cast<UEnemyHealthBarWidget>(GetUserWidgetObject());
    if (HPWidget)
    {
        HPWidget->UpdateHealthBar(CurrentHP / MaxHP);
    }

    // 일정 시간 후 자동으로 숨기는 타이머 설정
    GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UEnemyHealthBarComponent::HideHealthBar, DisplayDuration);
}

void UEnemyHealthBarComponent::HideHealthBar()
{
    SetVisibility(false);
}
