//StageDoor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageDoor.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API AStageDoor : public AActor
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintImplementableEvent)
    void OpenDoor();

    //  StageNumber 추가, 블루프린트에서 설정 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    int32 StageNumber = 1; // 기본값 1
};
