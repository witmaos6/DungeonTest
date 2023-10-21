// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonTest/Character/Aurora.h"

#include "DungeonTest/Components/SkillComponent.h"
#include "DungeonTest/Components/HealthComponent.h"
#include "DungeonTest/Components/MPComponent.h"
#include "DungeonTest/CharacterBase/BasicPlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

AAurora::AAurora()
{
	AttackRadius = 300.0f;
	AttackRange = 200.0f;
	BasicDamage = 15.0f;
	bPermitAttack = true;

	ChargeSkill = CreateDefaultSubobject<USkillComponent>("ChargeSkill");
	CastingSkill = CreateDefaultSubobject<USkillComponent>("CastingSkill");
	EvadeSkill = CreateDefaultSubobject<USkillComponent>("EvadeSkill");
	FreezeSkill = CreateDefaultSubobject<USkillComponent>("FreezeSkill");

	CharacterStatus = ECharacterStatus::ECS_Begin;
}

void AAurora::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();

	PlayerController = Cast<ABasicPlayerController>(GetController());

	if (!HasAuthority())
	{
		ServerPlayMontage(BeginMontage, FName("Begin"), 1.0f);

		const USkeletalMeshSocket* FloorSocket = GetMesh()->GetSocketByName("FloorSocket");
		if (FloorSocket)
		{
			FVector SocketLocation = FloorSocket->GetSocketLocation(GetMesh());
			FRotator SocketRotation = GetMesh()->GetSocketRotation("FloorSocket");
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeginParticle, SocketLocation, SocketRotation, true);
		}
	}

	AttackStatus = EAttackStatus::EAS_Normal;

	AttackIgnoreActor.Add(this);

	HealthComponent->OnHealthChanged.AddDynamic(this, &AAurora::OnHealthChanged);
}

void AAurora::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAurora::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &AAurora::BasicAttack);
	PlayerInputComponent->BindAction("BasicAttack", IE_Repeat, this, &AAurora::BasicAttack);

	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &AAurora::Charging);
	PlayerInputComponent->BindAction("Charge", IE_Released, this, &AAurora::ChargeAttack);

	PlayerInputComponent->BindAction("Casting", IE_Pressed, this, &AAurora::CastingKeyDown);
	PlayerInputComponent->BindAction("Casting", IE_Released, this, &AAurora::CastingStart);

	PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &AAurora::Evade);

	PlayerInputComponent->BindAction("Freeze", IE_Pressed, this, &AAurora::FreezeAttack);
}

void AAurora::BasicAttack()
{
	if (!bPermitAttack)
		return;

	if (!(CharacterStatus == ECharacterStatus::ECS_Normal || CharacterStatus == ECharacterStatus::ECS_Attack))
	{
		return;
	}

	CharacterStatus = ECharacterStatus::ECS_Attack;

	if (AnimInstance && BasicAttackMontage)
	{
		switch (AttackStatus)
		{
		case EAttackStatus::EAS_Normal:
			AttackStatus = EAttackStatus::EAS_FirstAttack;
			if (!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("FirstAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange, AttackRadius);
				ServerPlaySound(BasicAttackSound);
			}
			break;

		case EAttackStatus::EAS_FirstAttack:
			AttackStatus = EAttackStatus::EAS_SecondAttack;
			if (!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("SecondAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange, AttackRadius);
				ServerPlaySound(BasicAttackSound);
			}
			break;

		case EAttackStatus::EAS_SecondAttack:
			AttackStatus = EAttackStatus::EAS_ThirdAttack;
			if (!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("ThirdAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange, AttackRadius);
				ServerPlaySound(BasicAttackSound);
			}
			break;
		default:
			AttackStatus = EAttackStatus::EAS_Normal;
			break;
		}
	}
}

void AAurora::Charging()
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal && ChargeSkill->IsAvailable())
	{
		PlayerController->VisibleSkillGage(ChargeSkill->GetGageIncreasingSpeed(), ChargeSkill->GetGageLoops());

		ChargeSkill->SetGageTimer();
		ChargeSkill->bGageIncreasing = true;

		CharacterStatus = ECharacterStatus::ECS_Rotation;

		ChargeSkill->bKeyDown = true;

		if (!HasAuthority())
		{
			ServerPlayMontage(ChargeSkill->SkillMontage, FName("Charging"), ChargeSkill->AnimPlaySpeeds[0]);
		}

		float ForcedExcuteTime = ChargeSkill->GetExcuteTime(1.0f);
		GetWorldTimerManager().SetTimer(ChargeTimer, this, &AAurora::ChargeAttack, ForcedExcuteTime);
	}
}

void AAurora::ChargeAttack()
{
	if (ChargeSkill->bKeyDown)
	{
		GetWorldTimerManager().ClearTimer(ChargeTimer);

		PlayerController->RemoveSkillGage();
		ChargeSkill->bGageIncreasing = false;

		float ChargeDamage = ChargeSkill->GetChargeDamage();
		if (!HasAuthority())
		{
			ServerPlayMontage(ChargeSkill->SkillMontage, FName("Attack"), ChargeSkill->AnimPlaySpeeds[1]);
			ServerSpawnParticle(ChargeSkill->SkillParticles[0], GetActorLocation(), GetActorRotation());
			ServerPlaySound(ChargeSkill->SkillSounds[0]);
			ServerApplyDamageBasic(ChargeDamage, ChargeSkill->AttackDistance, ChargeSkill->AttackRadius);
		}

		ChargeSkill->bKeyDown = false;
		ChargeSkill->ApplyCoolDown();

		MPComponent->ReduceMana(ChargeSkill->GetRequireMana());
	}
}

void AAurora::CastingKeyDown()
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal && CastingSkill->IsAvailable())
	{
		CharacterStatus = ECharacterStatus::ECS_Attack;

		CastingSkill->bKeyDown = true;
	}
}

void AAurora::CastingStart()
{
	if (CastingSkill->bKeyDown)
	{
		PlayerController->VisibleSkillGage(CastingSkill->GetGageIncreasingSpeed(), CastingSkill->GetGageLoops());

		CastingSkill->bKeyDown = false;

		if (!HasAuthority())
		{
			ServerPlayMontage(CastingSkill->SkillMontage, FName("Casting"), CastingSkill->AnimPlaySpeeds[0]);
			ServerSpawnParticle(CastingSkill->SkillParticles[0], GetActorLocation(), GetActorRotation());
		}

		GetWorldTimerManager().SetTimer(CastingTimer, this, &AAurora::CastingAttack, CastingSkill->GetExcuteTime(0.0f));
	}
}

void AAurora::CastingAttack()
{
	PlayerController->RemoveSkillGage();

	GetWorldTimerManager().ClearTimer(CastingTimer);

	if (!HasAuthority())
	{
		ServerPlayMontage(CastingSkill->SkillMontage, FName("Attack"), CastingSkill->AnimPlaySpeeds[1]);
		ServerSpawnParticle(CastingSkill->SkillParticles[1], GetActorLocation(), GetActorRotation());
		ServerPlaySound(CastingSkill->SkillSounds[0]);
		ServerApplyDamageBasic(CastingSkill->SkillDamage, CastingSkill->AttackDistance, CastingSkill->AttackRadius);
	}
	CastingSkill->ApplyCoolDown();

	MPComponent->ReduceMana(CastingSkill->GetRequireMana());
}

void AAurora::FreezeAttack()
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal && FreezeSkill->IsAvailable())
	{
		CharacterStatus = ECharacterStatus::ECS_Attack;

		if (!HasAuthority())
		{
			ServerPlayMontage(FreezeSkill->SkillMontage, FName("Freeze"), FreezeSkill->AnimPlaySpeeds[0]);
			ServerApplyDamageBasic(FreezeSkill->SkillDamage, FreezeSkill->AttackDistance, FreezeSkill->AttackRadius);
			ServerPlaySound(FreezeSkill->SkillSounds[0]);
			ServerSpawnParticle(FreezeSkill->SkillParticles[0], GetActorLocation(), FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
		}
		FreezeSkill->ApplyCoolDown();

		MPComponent->ReduceMana(FreezeSkill->GetRequireMana());
	}
}

void AAurora::Evade()
{
	if (EvadeSkill->IsAvailable() && CharacterStatus != ECharacterStatus::ECS_Begin)
	{
		CharacterStatus = ECharacterStatus::ECS_Evade;

		if (!HasAuthority())
		{
			ServerPlayMontage(EvadeSkill->SkillMontage, FName("Evade"), EvadeSkill->AnimPlaySpeeds[0]);
			ServerLaunchCharacter();
		}
		EvadeSkill->ApplyCoolDown();

		MPComponent->ReduceMana(EvadeSkill->GetRequireMana());
	}
}

void AAurora::OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && CharacterStatus != ECharacterStatus::ECS_Dead)
	{
		CharacterStatus = ECharacterStatus::ECS_Dead;
		GetMovementComponent()->StopMovementImmediately();
		DisableInput(PlayerController);
		if (HasAuthority())
		{
			MulticastPlayMontage(DeathMontage, FName("Death"), 0.5f);
		}
		SetLifeSpan(2.5f);
	}
}
