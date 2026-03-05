// Fill out your copyright notice in the Description page of Project Settings.


#include "Team7_CH3_Project/Weapon/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GrenadeProjectile.h"
#include "GrenadeStat.h"
#include "Enemy/IEntityStats.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h" // KH 추가 : UI
#include "Team7_CH3_Project/UI/DevHHUD.h" // KH 추가 : UI
#include "Team7_CH3_Project/UI/DevHCrosshairWidget.h" // KH 추가 : UI
#include "Team7_CH3_Project/UI/DevHHUD.h" // KH 260225 추가 : 로그

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// 쓰레기 값으로 인한 사격 차단 방지
	LastFireTime = 0.0f;
	LastWeaponSwitchTime = -WeaponSwitchDelay;

	// 데이터 테이블 로드 및 초기 무기 세팅
	if (WeaponStatTable)
	{
		CurrentStat = WeaponStatTable->FindRow<FWeaponStat>(WeaponRowName, TEXT(""));
	}

	// 가져온 정보가 있다면 초기화 진행
	if (CurrentStat)
	{
		CurrentAmmo = CurrentStat->MaxAmmo;
		// 초기 무기 탄약 등록
		WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponStat NOT FOUND! Check Table or RowName."));
	}

	bIsNextShotLeft = true;

    // KH 추가 : 시작 시 UI에 초기 무기 정보 방송
    if (CurrentStat)
    {
        if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
        {
            UISub->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
        }
    }

	if (GrenadeTable)
	{
		FGrenadeStat* Stat = GrenadeTable->FindRow<FGrenadeStat>(GrenadeRowName, TEXT(""));
		if (Stat)
		{
			// 데이터 테이블의 MaxCharges(3)를 가져옴
			CurrentGrenadeCount = Stat->MaxCharges;
			UE_LOG(LogTemp, Log, TEXT("Initial Grenades: %d"), CurrentGrenadeCount);
		}
	}
}

void UWeaponComponent::StartFire()
{
	bHoldingFire = true; // 마우스를 눌렀음을 기록

	// 상태 가드: Idle 상태이거나 이미 Firing 중일 때만 사격 루프 진입 가능
	if (CurrentState != EWeaponState::Idle && CurrentState != EWeaponState::Firing) return;

	if (!CurrentStat) return;

	// 탄약이 없으면 즉시 장전 시도
	if (CurrentAmmo <= 0)
	{
		StartReload();
		return;
	}

	// 상태 확정 : 실제로 사격 상태
	CurrentState = EWeaponState::Firing;

	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 연사 간격 보정 (즉시 발사 가능 여부 판단)
	if (CurrentTime - LastFireTime >= CurrentStat->FireRate)
	{
		// 충분한 시간이 지났다면 즉시 발사 가능하도록 LastFireTime을 과거로 설정
		LastFireTime = CurrentTime - CurrentStat->FireRate;
	}

	// 첫 발 즉시 발사 후 FireRate 간격으로 자동 연사 타이머 작동
	Fire();

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UWeaponComponent::Fire,
		CurrentStat->FireRate,
		true
	);
}

void UWeaponComponent::StopFire()
{
	bHoldingFire = false; // 마우스를 뗐음을 기록
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	// 상태 복구 : 사격 중이었다면 다시 Idle로 변경
	// 장전 중이거나 무기 교체 중에 마우스를 뗀 것이라면 해당 상태를 유지해야 함
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
		UE_LOG(LogTemp, Log, TEXT("사격 중단"));
	}
}

void UWeaponComponent::Fire()
{
	// 사격 가능 상태 검증
	if (!CanFire())
	{
		if (CurrentAmmo <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
			if (CurrentState == EWeaponState::Firing) CurrentState = EWeaponState::Idle;
			StartReload();
		}

		return;
	}

	CurrentState = EWeaponState::Firing;
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	if (CurrentStat->FireCameraShake)
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController()))
		{
			PC->ClientStartCameraShake(CurrentStat->FireCameraShake);
		}
	}

	if (CurrentStat && CurrentStat->FireMontage)
	{
		UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 몽타주 재생 (이미 재생 중이면 리셋됨)
			OwnerChar->PlayAnimMontage(CurrentStat->FireMontage);
			// bIsNextShotLeft 값에 따라 Fire_L 또는 Fire_R 섹션으로 점프
			FName SectionName = bIsNextShotLeft ? FName("Fire_L") : FName("Fire_R");
			AnimInstance->Montage_JumpToSection(SectionName, CurrentStat->FireMontage);
		}
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 발사 지점 및 방향 설정
	FName TargetSocketName = bIsNextShotLeft ? LeftMuzzleSocketName : RightMuzzleSocketName;
	FVector Start = OwnerChar->GetMesh()->GetSocketLocation(TargetSocketName);
	FVector FireDir = GetFireDirection(Start);

	// 발사 피드백
	PlayFireEffects(Start, TargetSocketName);

	// 탄환 판정
	int32 TotalPellets = FMath::Max(1, CurrentStat->PelletCount);
	for (int32 i = 0; i < TotalPellets; i++)
	{
		float SpreadRad = FMath::DegreesToRadians(CurrentStat->BulletSpread);
		FVector End = Start + (FMath::VRandCone(FireDir, SpreadRad) * CurrentStat->Range);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerChar);

		// LineTrace 판정 및 ProcessHit 호출 (데미지/로그/히트마커 처리는 ProcessHit 내부에서 수행)
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			ProcessHit(Hit);
		}

		// 트레이서(궤적) 생성
		if (CurrentStat->TracerEffect)
		{
			FVector FinalEnd = Hit.bBlockingHit ? Hit.ImpactPoint : End;

			UParticleSystemComponent* Tracer = UGameplayStatics::SpawnEmitterAttached(
				CurrentStat->TracerEffect,
				OwnerChar->GetMesh(),
				TargetSocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget
			);

			if (Tracer) Tracer->SetVectorParameter(TEXT("Target"), FinalEnd);
		}
	}

	// 데이터 업데이트 및 로그
	CurrentAmmo--;
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);
	LastFireTime = CurrentTime;
	bIsNextShotLeft = !bIsNextShotLeft;

	UE_LOG(LogTemp, Log, TEXT("남은 탄약: %d / %d"), CurrentAmmo, CurrentStat->MaxAmmo);

	// KH 추가 : UI 서브시스템 방송 (원문 유지)
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UDevHUISubSystem* UISubSystem = GI->GetSubsystem<UDevHUISubSystem>())
		{
			UISubSystem->BroadcastNormalAttack(CurrentStat->FireRate);
			UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
		}
	}

	// 탄약 소진 시 후처리
	if (CurrentAmmo <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		StartReload();
	}
}

void UWeaponComponent::StartReload()
{
	// 상태 체크 : 이미 장전 중이거나 무기 교체 중이면 무시
	if (CurrentState == EWeaponState::Reloading || CurrentState == EWeaponState::Swapping) return;

	// 탄약이 이미 꽉 찼다면 무시
	if (CurrentAmmo >= CurrentStat->MaxAmmo) return;

	// 상태 전환 : 장전 상태로 변경 (사격 등을 차단)
	CurrentState = EWeaponState::Reloading;

    // KH 추가 - 260223 :  UI 서브시스템에 재장전 시간 방송
    if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        UISub->BroadcastReload(CurrentStat->ReloadTime);
    }

	// 장전 사운드 재생
	if (CurrentStat->ReloadSound)
	{
		// 캐릭터의 위치에서 사운드를 재생
		UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->ReloadSound, GetOwner()->GetActorLocation());
	}

	UE_LOG(LogTemp, Log, TEXT("장전 중... 예상 시간: %f"), CurrentStat->ReloadTime);

	// ReloadTime 후에 CompleteReload 함수를 호출하도록 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle,
			this,
			&UWeaponComponent::CompleteReload,
			CurrentStat->ReloadTime,
			false
	);
}

void UWeaponComponent::CompleteReload()
{
	// 데이터 업데이트
	CurrentAmmo = CurrentStat->MaxAmmo;
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);

    // KH 260225 추가 : 재장전 시 로그
    if (ADevHHUD* HUD = Cast<ADevHHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
    {
        HUD->AddGameLog(FName("Weapon_Reload"));
    }

	// 상태 해제 : 다시 Idle 상태로 변경
	CurrentState = EWeaponState::Idle;

    // UI 서브 시스템
    if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
    }

	float CurrentTime = GetWorld()->GetTimeSeconds();
	LastFireTime = CurrentTime - CurrentStat->FireRate;

	UE_LOG(LogTemp, Log, TEXT("장전 완료"));

	// 장전이 끝났을 때 마우스를 여전히 누르고 있다면 사격 재개
	if (bHoldingFire)
	{
		StartFire();
	}
}

void UWeaponComponent::ChangeWeapon(FName NewWeaponName)
{
	// 상태 및 중복 체크
	if (CurrentState == EWeaponState::Reloading || CurrentState == EWeaponState::Swapping || WeaponRowName == NewWeaponName)
	{
		UE_LOG(LogTemp, Warning, TEXT("장전/교체 중에는 무기를 바꿀 수 없거나 이미 같은 무기입니다."));
		return;
	}

	if (!WeaponStatTable) return;

	// 무기 교체 상태로 전환
	CurrentState = EWeaponState::Swapping;

	// 현재 무기의 남은 탄약을 맵에 저장
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);

	//  KH 추가 : 새로운 행 이름으로 데이터 찾기
	FWeaponStat* NewStat = WeaponStatTable->FindRow<FWeaponStat>(NewWeaponName, TEXT(""));

    if (NewStat)
    {
        //  KH 추가 : 헬퍼 함수를 사용하여 간결하게 체크
        if (GetCurrentScore() < NewStat->UnlockScore)
        {
            UE_LOG(LogTemp, Warning, TEXT("무기 잠김 상태: 해금 점수 부족!"));
			CurrentState = EWeaponState::Idle;
            return;
        }

		// 현재 스탯 교체 및 변수 초기화
		WeaponRowName = NewWeaponName;
		CurrentStat = NewStat;

		//  새로운 무기의 탄약을 맵에서 불러오기
		if (WeaponAmmoMap.Contains(NewWeaponName))
		{
			CurrentAmmo = WeaponAmmoMap[NewWeaponName];
		}
		else
		{
			// 맵에 정보가 없다면(처음 드는 무기라면) 최대 탄약으로 초기화
			CurrentAmmo = CurrentStat->MaxAmmo;
			WeaponAmmoMap.Add(NewWeaponName, CurrentAmmo);
		}

		float CurrentTime = GetWorld()->GetTimeSeconds();
		LastFireTime = CurrentTime;
		LastWeaponSwitchTime = CurrentTime;

		// 지연 시간 후 상태 복구 로직
		// 사격 중이었다면 중단 후 예약, 아니면 그냥 Idle로 복귀
		if (bHoldingFire) StopFire();

		FTimerHandle SwapTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			SwapTimerHandle,
			[this]()
			{
				if (!IsValid(this)) return;
				CurrentState = EWeaponState::Idle;
				UE_LOG(LogTemp, Log, TEXT("무기 교체 완료 : Idle 상태로 복귀"));

				// 마우스를 계속 누르고 있었다면 사격 재개
				if (bHoldingFire)
				{
					StartFire();
				}
			},
			WeaponSwitchDelay,
			false
		);

        // KH 추가 : 성공적으로 교체되었을 때만 UI 방송
        if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
        {
            UISubSystem->TriggerWeaponSelection(NewStat->SlotIndex);

            // KH 추가 : 무기 이름과 탄약 정보 방송 & 초기 무기의 슬롯 인덱스 선택 신호 방송
            UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
            UISubSystem->TriggerWeaponSelection(CurrentStat->SlotIndex);
        }
	}
}

int32 UWeaponComponent::GetCurrentScore() const
{
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            //  KH 추가 : 서브시스템에 구현된 GetCurrentScore()를 호출
            return UISub->GetCurrentScore();
        }
    }
    return 0;
}

void UWeaponComponent::LaunchGrenade()
{
	if (!GrenadeTable)
	{
		UE_LOG(LogTemp, Error, TEXT("GrenadeTable is NULL"));
		return;
	}
	// 데이터 테이블에서 설정값 가져오기
	FGrenadeStat* Stat = GrenadeTable->FindRow<FGrenadeStat>(GrenadeRowName, TEXT(""));
	if (!Stat || !Stat->GrenadeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find Row or GrenadeClass is NULL"));
		return;
	}

	// 남은 개수 체크 (0개 이하면 발사 안 함)
	if (CurrentGrenadeCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("수류탄이 없습니다"));
		return;
	}

	// 쿨타임 체크
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastGrenadeTime < Stat->CooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("수류탄 재사용 대기 시간 : %.1f"),
			Stat->CooldownTime - (CurrentTime - LastGrenadeTime));
		return;
	}

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar || !OwnerChar->GetMesh()) return;

    // KH 추가 - 260223 : 클릭 시 무기 정보 UI만 수류탄으로 먼저 교체
    if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        UISubSystem->BroadcastWeaponStatus(TEXT("Grenade"), CurrentGrenadeCount, Stat->MaxCharges);
    }

	if (Stat->GrenadeMontage)
	{
		OwnerChar->PlayAnimMontage(Stat->GrenadeMontage);
	}
	else
	{
		// 등록된 애니메이션이 없다면 예외적으로 즉시 발사합니다.
		ExecuteLaunch();
	}
}

void UWeaponComponent::ExecuteLaunch()
{
	// 실제 물체를 소환하고 데이터를 갱신하는 핵심 로직입니다.
	if (!GrenadeTable) return;
	FGrenadeStat* Stat = GrenadeTable->FindRow<FGrenadeStat>(GrenadeRowName, TEXT(""));
	if (!Stat) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 1. 소환 위치 및 방향 계산
	FName SocketName = TEXT("grenade_front");
	FVector SpawnLocation = OwnerChar->GetMesh()->GetSocketLocation(SocketName);
	SpawnLocation += OwnerChar->GetActorForwardVector() * 20.f;
	FRotator SpawnRotation = OwnerChar->GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = OwnerChar;
	Params.Instigator = OwnerChar;

	// 2. 수류탄 소환
	AActor* SpawnedActor = World->SpawnActor<AActor>(Stat->GrenadeClass, SpawnLocation, SpawnRotation, Params);

	if (SpawnedActor)
	{
		CurrentGrenadeCount--;
		LastGrenadeTime = GetWorld()->GetTimeSeconds();

		// 3. UI 및 리젠 타이머 로직
		if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
		{
			UISubSystem->BroadcastWeaponStatus(TEXT("Grenade"), CurrentGrenadeCount, Stat->MaxCharges);
			UISubSystem->BroadcastSkillAttack(Stat->CooldownTime);

			FTimerHandle RestoreHandle;
			GetWorld()->GetTimerManager().SetTimer(RestoreHandle, [this, UISubSystem, Stat]()
				{
					if (GetWorld()->GetTimerManager().IsTimerActive(GrenadeRegenTimerHandle))
					{
						float CurrentRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(GrenadeRegenTimerHandle);
						UISubSystem->BroadcastGrenadeRegen(CurrentRemaining);
					}
				}, Stat->CooldownTime, false);
		}

		// 리젠 타이머 시작
		if (!GetWorld()->GetTimerManager().IsTimerActive(GrenadeRegenTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(GrenadeRegenTimerHandle, this, &UWeaponComponent::RegenerateGrenade, Stat->RegenTime, true);
		}

		// 4. 수류탄 프로젝트일 설정 (스탯 및 콜리전)
		AGrenadeProjectile* Projectile = Cast<AGrenadeProjectile>(SpawnedActor);
		if (Projectile)
		{
			// 몬스터 콜리전 무시 방지: PhysicsBody 채널 설정 및 Pawn 차단
			UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
			if (RootPrimitive)
			{
				RootPrimitive->SetCollisionObjectType(ECC_PhysicsBody);
				RootPrimitive->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
			}

			// 스탯 전달
			Projectile->MaxDamage = Stat->MaxDamage;
			Projectile->MinDamage = Stat->MinDamage;
			Projectile->InnerRadius = Stat->InnerRadius;
			Projectile->ExplosionRadius = Stat->ExplosionRadius;
			Projectile->ImpulseStrength = Stat->ImpulseStrength;
			Projectile->ExplosionDecal = Stat->ExplosionDecal;
			Projectile->ExplosionEffect = Stat->ExplosionEffect;
			Projectile->ExplosionSound = Stat->ExplosionSound;
			Projectile->ExplosionCameraShake = Stat->ExplosionCameraShake;

			// 발사 물리 속도 적용
			UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>();
			if (Movement)
			{
				FVector LaunchDir = GetFireDirection(SpawnLocation);
				Movement->Velocity = LaunchDir * Stat->LaunchForce;
			}
		}

		// 5. 발사 피드백 (사운드 및 이펙트)
		if (Stat->LaunchSound) UGameplayStatics::PlaySoundAtLocation(World, Stat->LaunchSound, SpawnLocation);
		if (Stat->MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAttached(
				Stat->MuzzleFlash,
				OwnerChar->GetMesh(),
				TEXT("grenade_front"),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget
			);
		}
		if (Stat->FireCameraShake)
		{
			if (APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController()))
			{
				PC->ClientStartCameraShake(Stat->FireCameraShake);
			}
		}
	}
}

void UWeaponComponent::RegenerateGrenade()
{
	if (!GrenadeTable) return;

	FGrenadeStat* Stat = GrenadeTable->FindRow<FGrenadeStat>(GrenadeRowName, TEXT(""));
	if (!Stat) return;

	// 개수 증가
	CurrentGrenadeCount++;
	UE_LOG(LogTemp, Log, TEXT("수류탄 1개 충전됨 현재 개수 : %d / %d"), CurrentGrenadeCount, Stat->MaxCharges);

    // KH 260225 추가 : 수류탄 충전 시 로그
    if (ADevHHUD* HUD = Cast<ADevHHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
    {
        HUD->AddGameLog(FName("Grenade_Regen"));
    }

    // KH 추가 - 260223 : 아직 최대치가 아니라면 다음 충전 시간 UI 방송
    if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        // 충전된 개수 반영
        UISub->BroadcastWeaponStatus(TEXT("Grenade"), CurrentGrenadeCount, Stat->MaxCharges);

        // 아직 최대치가 아니라면 다음 수류탄 충전을 위해 리젠 바 다시 시작
        if (CurrentGrenadeCount < Stat->MaxCharges)
        {
            UISub->BroadcastGrenadeRegen(Stat->RegenTime);
        }
    }

	// 최대치까지 찼다면 타이머 종료
	if (CurrentGrenadeCount >= Stat->MaxCharges)
	{
		CurrentGrenadeCount = Stat->MaxCharges;
		GetWorld()->GetTimerManager().ClearTimer(GrenadeRegenTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("수류탄 충전 완료."));
	}
}

bool UWeaponComponent::CanFire() const
{
	// 상태 체크
	// Idle이거나 이미 사격 중(Firing)일 때만 추가 발사 가능
	if (CurrentState != EWeaponState::Idle && CurrentState != EWeaponState::Firing) return false;

	// Grenade 몽타주 재생 여부 체크
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar && OwnerChar->GetMesh())
	{
		UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
		if (AnimInstance && GrenadeTable)
		{
			FGrenadeStat* GStat = GrenadeTable->FindRow<FGrenadeStat>(GrenadeRowName, TEXT(""));
			// 수류탄 몽타주가 등록되어 있고, 현재 그 몽타주가 재생 중이라면 사격 차단
			if (GStat && GStat->GrenadeMontage && AnimInstance->Montage_IsPlaying(GStat->GrenadeMontage))
			{
				return false;
			}
		}
	}

	// 기본 스탯 및 탄약 체크
	if (!CurrentStat || CurrentAmmo <= 0) return false;

	// 점수 해금 체크
	if (GetCurrentScore() < CurrentStat->UnlockScore) return false;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	// 무기 교체 지연 및 연사 속도 체크
	if (CurrentTime - LastWeaponSwitchTime < WeaponSwitchDelay) return false;
	if (CurrentTime - LastFireTime < CurrentStat->FireRate * 0.8f) return false;

	return true;
}

FVector UWeaponComponent::GetFireDirection(const FVector& StartPos) const
{
	APlayerController* PC = Cast<APlayerController>(Cast<ACharacter>(GetOwner())->GetController());
	if (!PC) return GetOwner()->GetActorForwardVector();

	FHitResult CursorHit; // 커서 아래의 물리적 히트 지점
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	// ECC_Visibility 채널을 사용하여 바닥이나 적 등 눈에 보이는 물체를 감지합니다.
	if (PC->GetHitResultUnderCursorByChannel(TraceChannel, true, CursorHit))
	{
		FVector CharPos = GetOwner()->GetActorLocation();
		FVector TargetPoint = CursorHit.ImpactPoint;

		// 캐릭터에서 타겟으로 향하는 수평 벡터 계산 (Z값 무시)
		FVector ToTarget = TargetPoint - CharPos;
		ToTarget.Z = 0.f;

		// 조준 원의 반지름 설정 (이 범위 안에서는 사격 각도를 원의 경계로 고정)
		float AimCircleRadius = 250.f;
		float CurrentDist = ToTarget.Size();

		// 만약 마우스가 원 안에 있다면, 방향만 유지한 채 길이를 반지름만큼 늘림
		if (CurrentDist < AimCircleRadius)
		{
			// 마우스가 캐릭터와 완전히 겹쳐서 방향을 알 수 없는 경우(Size가 0에 가까울 때) 예외 처리
			if (ToTarget.IsNearlyZero())
			{
				ToTarget = GetOwner()->GetActorForwardVector() * AimCircleRadius;
			}
			else
			{
				ToTarget = ToTarget.GetSafeNormal() * AimCircleRadius;
			}
		}
		// 최종 타겟 지점 재설정 (캐릭터 위치 + 원 위의 벡터 + 가슴 높이 보정)
		FVector FinalTargetPoint = CharPos + ToTarget;
		FinalTargetPoint.Z = CharPos.Z + 100.f;

		// 총구에서 이 최종 지점을 바라보는 방향 반환
		FVector FinalDir = (FinalTargetPoint - StartPos).GetSafeNormal();
		FinalDir.Z = 0.f;

		return FinalDir.GetSafeNormal();
	}
	return GetOwner()->GetActorForwardVector();
}

void UWeaponComponent::ProcessHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !IsValid(HitActor)) return;

	// 시각/청각 효과
	if (CurrentStat->HitSound) UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->HitSound, Hit.ImpactPoint);
	if (CurrentStat->ImpactEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurrentStat->ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

	// 데미지 적용 및 UI 피드백 (인터페이스 활용)
	if (IEntityStats* Enemy = Cast<IEntityStats>(HitActor))
	{
		Enemy->TakeDamage(CurrentStat->Damage);
		UE_LOG(LogTemp, Log, TEXT("몬스터 타격 Damage : %f"), CurrentStat->Damage);
		// KH 추가 : 히트마커 애니메이션
		APlayerController* PC = Cast<APlayerController>(Cast<ACharacter>(GetOwner())->GetController());
		if (PC)
		{
			if (ADevHHUD* DevHUD = Cast<ADevHHUD>(PC->GetHUD()))
			{
				if (UDevHCrosshairWidget* Crosshair = DevHUD->GetCrosshairWidget())
				{
					Crosshair->PlayHitMarker();
				}
			}
		}
	}
	else
	{
		// 일반 사물 데미지
		UGameplayStatics::ApplyDamage(HitActor, CurrentStat->Damage, nullptr, GetOwner(), nullptr);
	}
}

void UWeaponComponent::PlayFireEffects(const FVector& StartPos, const FName& SocketName)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar || !OwnerChar->GetMesh()) return;

	if (CurrentStat->MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(
			CurrentStat->MuzzleFlash,
			OwnerChar->GetMesh(),
			SocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget // 소켓에 딱 붙임
		);
	}

	if (CurrentStat->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->FireSound, StartPos);
	}
}
