#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameLogTypes.generated.h"


UENUM(BlueprintType)
enum class ELogType : uint8
{
    Default     UMETA(DisplayName = "일반"),
    Kill        UMETA(DisplayName = "처치"),
    Unlock      UMETA(DisplayName = "해금"),
    System      UMETA(DisplayName = "시스템")
};

USTRUCT(BlueprintType)
struct FLogData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Log")
    FText LogName; // 로그 제목

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Log")
    FText LogContent; // 로그 상세 내용

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Log")
    ELogType LogType; // 로그 표시 타입
};
