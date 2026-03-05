// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Enemy/IEntityStats.h"
#include "DrawDebugHelpers.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 충돌체 생성 및 루트 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	// 콜리전 설정 (발사체 표준)
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	SphereComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	// 외형(Mesh) 생성 및 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬는 충돌 무시 (구체만 충돌)

	// 발사체 이동 컴포넌트 생성
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// 어깨 런처용 초기 설정
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 2000.f; // 데이터 테이블에서 덮어쓰기 전 기본값
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향 조준
	ProjectileMovement->bShouldBounce = false;           // 벽에 닿으면 튕기지 않고 OnHit 발생
	ProjectileMovement->ProjectileGravityScale = 0.3f;   // 살짝만 아래로 휘게 (런처 느낌)
}

// Called when the game starts or when spawned
void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		if (SphereComp)
		{
			SphereComp->IgnoreActorWhenMoving(MyOwner, true);
		}

		if (MyOwner->GetRootComponent())
		{
			UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(MyOwner->GetRootComponent());
			if (OwnerRoot)
			{
				OwnerRoot->IgnoreActorWhenMoving(this, true);
			}
		}
	}
	// 충돌 이벤트 바인딩
	if (SphereComp)
	{
		SphereComp->OnComponentHit.AddDynamic(this, &AGrenadeProjectile::OnHit);
	}
}

void AGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 본인(캐릭터)과의 충돌은 무시 나 자신도 아닐 때만 터짐
	if (OtherActor && OtherActor != GetOwner() && OtherActor != this)
	{
		Explode();
	}
}

void AGrenadeProjectile::Explode()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector Start = GetActorLocation();


	// 시각/청각 효과
	if (ExplosionEffect) UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionEffect, Start, GetActorRotation());
	if (ExplosionSound) UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, Start);

	// 몬스터 감지 및 데미지 전달
	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	float DamageRange = ExplosionRadius - InnerRadius;
	if (DamageRange <= 0.0f) DamageRange = 1.0f;

	bool bHasOverlap = World->SweepMultiByChannel(
		HitResults, Start, Start, FQuat::Identity,
		ECC_Pawn,
		SphereShape);

	if (bHasOverlap)
	{
		TArray<AActor*> DamagedActors;

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !DamagedActors.Contains(HitActor) && HitActor != GetOwner())
			{
				// 인터페이스 캐스팅
				IEntityStats* Enemy = Cast<IEntityStats>(HitActor);
				if (Enemy)
				{
					float Distance = FVector::Dist(Start, HitActor->GetActorLocation());

					// 거리별 데미지 보간 계산
					float Alpha = FMath::Clamp((ExplosionRadius - Distance) / DamageRange, 0.0f, 1.0f);
					float FinalDamage = FMath::Lerp(MinDamage, MaxDamage, Alpha);

					// 인터페이스 함수 호출
					Enemy->TakeDamage(FinalDamage);
					DamagedActors.Add(HitActor);
				}

				// 물리 효과 (WorldDynamic이고 시뮬레이션 중인 경우)
				if (UPrimitiveComponent* HitComp = Hit.GetComponent())
				{
					if (HitComp->IsSimulatingPhysics())
					{
						HitComp->AddRadialImpulse(Start, ExplosionRadius, ImpulseStrength, RIF_Linear, true);
					}
				}
			}
		}
	}

	// 물리적인 밀쳐내기 효과 추가
	HitResults.Empty(); // 기존 결과 비우기
	if (World->SweepMultiByChannel(HitResults, Start, Start, FQuat::Identity, ECC_WorldDynamic, SphereShape))
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (UPrimitiveComponent* HitComp = Hit.GetComponent())
			{
				if (HitComp->IsSimulatingPhysics())
				{
					HitComp->AddRadialImpulse(Start, ExplosionRadius, ImpulseStrength, RIF_Linear, true);
				}
			}
		}
	}

	// 데칼 생성
	if (ExplosionDecal)
	{
		FHitResult GroundHit;
		FVector TraceEnd = Start + (FVector::DownVector * 500.f);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		if (GetOwner()) Params.AddIgnoredActor(GetOwner());

		if (World->LineTraceSingleByChannel(GroundHit, Start, TraceEnd, ECC_Visibility, Params))
		{
			UGameplayStatics::SpawnDecalAtLocation(World, ExplosionDecal, FVector(ExplosionRadius), GroundHit.ImpactPoint, GroundHit.ImpactNormal.Rotation(), 10.0f);
		}
	}

	// 카메라 쉐이크 실행
	if (ExplosionCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(World, ExplosionCameraShake, Start, 0.f, ExplosionRadius * 2.f);
	}

	// 시각적 디버깅 (범위 확인용)
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f);

	Destroy(); // 폭발 후 제거
}
