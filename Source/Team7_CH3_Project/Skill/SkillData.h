#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.generated.h"

/**
 * 스킬의 기본 정보와 쿨타임을 관리하는 구조체
 */
    USTRUCT(BlueprintType)
    struct FSkillData : public FTableRowBase
{
    GENERATED_BODY()

public:
    /* --- 기본 스탯 --- */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Damage = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Cooldown = 5.0f;

    /* --- UI 관련 --- */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UTexture2D* SkillIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FString SkillName = TEXT("Default Skill Name");

    /* --- 시각/청각 피드백 --- */

    // 스킬 발사 시 재생할 이펙트 (나이아가라)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    class UNiagaraSystem* SkillEffect = nullptr;

    // 스킬 발사 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> SkillSound = nullptr;
};
