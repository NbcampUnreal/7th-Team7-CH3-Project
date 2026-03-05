// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KirboActionComponent.generated.h"

class AKirboCharacter;
class UKirboStatComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM7_CH3_PROJECT_API UKirboActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKirboActionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 캐릭터와 스탯 컴포넌트 연결용 함수
	void Initialize(AKirboCharacter* InCharacter, UKirboStatComponent* InStatComp);

	UFUNCTION()
	void StartSprint();
	UFUNCTION()
	void StopSprint();
	UFUNCTION()
	void Dash();
	UFUNCTION()
	void StartShot();
	UFUNCTION()
	void StopShot();

	bool IsInvincible() const { return bIsInvincible; }
	void TriggerInvincibility(float Duration);

protected:
	UPROPERTY()
	AKirboCharacter* OwnerCharacter;
	UPROPERTY()
	UKirboStatComponent* StatComp;

	bool bIsInvincible = false;
	bool bCanDash = true;
	bool bIsShooting = false;

	FTimerHandle DashTimerHandle;
	FTimerHandle InvincibilityTimerHandle;

	void ResetDash();
	void ResetInvincibility();
};
