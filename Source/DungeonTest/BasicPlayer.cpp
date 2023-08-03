// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayer.h"

#include "BasicEnemy.h"
#include "BasicPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HealthComponent.h"
#include "SkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABasicPlayer::ABasicPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->TeamNumber = ETeamNumber::ETN_Player;

	AttackRadius = 300.0f;
	AttackRange = 200.0f;
	BasicDamage = 15.0f;
	bPermitAttack = true;

	ChargeSkill = CreateDefaultSubobject<USkillComponent>("ChargeSkill");
	CastingSkill = CreateDefaultSubobject<USkillComponent>("CastingSkill");

	CharacterStatus = ECharacterStatus::ECS_Begin;

	CurrentMaxGage = 0.0f;
	CurrentGage = 0.0f;
}

// Called when the game starts or when spawned
void ABasicPlayer::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
	PlayerController = Cast<ABasicPlayerController>(GetController());

	if(!HasAuthority())
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

	HealthComponent->OnHealthChanged.AddDynamic(this, &ABasicPlayer::OnHealthChanged);
}

// Called every frame
void ABasicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ChargeSkill->bGageIncreasing)
	{
		CurrentGage = ChargeSkill->GetGage();
	}
	else if(CastingSkill->bGageIncreasing)
	{
		CurrentGage = CastingSkill->GetGage();
	}
}

// Called to bind functionality to input
void ABasicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABasicPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasicPlayer::MoveRight);

	PlayerInputComponent->BindAxis("CameraTurn", this, &ABasicPlayer::AddControllerYawInput);

	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &ABasicPlayer::BasicAttack);
	PlayerInputComponent->BindAction("BasicAttack", IE_Repeat, this, &ABasicPlayer::BasicAttack);

	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &ABasicPlayer::Charging);
	PlayerInputComponent->BindAction("Charge", IE_Released, this, &ABasicPlayer::ChargeAttack);

	PlayerInputComponent->BindAction("Casting", IE_Pressed, this, &ABasicPlayer::CastingKeyDown);
	PlayerInputComponent->BindAction("Casting", IE_Released, this, &ABasicPlayer::CastingStart);
}

void ABasicPlayer::MoveForward(float Value)
{
	if(CharacterStatus == ECharacterStatus::ECS_Normal || CharacterStatus == ECharacterStatus::ECS_Move)
	{
		FRotator Rotator = FRotator(0, GetControlRotation().Yaw, 0);
		FVector Direction = FQuat(Rotator).GetForwardVector();

		AddMovementInput(Direction, Value);
	}	
}

void ABasicPlayer::MoveRight(float Value)
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal || CharacterStatus == ECharacterStatus::ECS_Move)
	{
		FRotator Rotation = FRotator(0, GetControlRotation().Yaw, 0);
		FVector Direction = FQuat(Rotation).GetRightVector();

		AddMovementInput(Direction, Value);
	}
	else if(CharacterStatus == ECharacterStatus::ECS_Rotation)
	{
		if(!HasAuthority())
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Yaw += Value * 2;
			ServerSetActorRotation(Rotation);
		}
	}
}

void ABasicPlayer::BasicAttack()
{
	if (!bPermitAttack)
		return;

	if(!(CharacterStatus == ECharacterStatus::ECS_Normal || CharacterStatus == ECharacterStatus::ECS_Attack))
	{
		return;
	}

	CharacterStatus = ECharacterStatus::ECS_Attack;

	if(AnimInstance && BasicAttackMontage)
	{
		switch (AttackStatus)
		{
		case EAttackStatus::EAS_Normal:
			AttackStatus = EAttackStatus::EAS_FirstAttack;
			if(!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("FirstAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange);
				ServerPlaySound(BasicAttackSound);
			}
			break;

		case EAttackStatus::EAS_FirstAttack:
			AttackStatus = EAttackStatus::EAS_SecondAttack;
			if (!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("SecondAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange);
				ServerPlaySound(BasicAttackSound);
			}
			break;

		case EAttackStatus::EAS_SecondAttack:
			AttackStatus = EAttackStatus::EAS_ThirdAttack;
			if (!HasAuthority())
			{
				ServerPlayMontage(BasicAttackMontage, FName("ThirdAttack"), 1.5f);
				ServerApplyDamageBasic(BasicDamage, AttackRange);
				ServerPlaySound(BasicAttackSound);
			}
			break;
		default:
			AttackStatus = EAttackStatus::EAS_Normal;
			break;
		}
	}
}

void ABasicPlayer::Charging()
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal && ChargeSkill->GetCoolState() <= 0.0f)
	{
		PlayerController->VisibleSkillGage();

		SetCurrentMaxGage(ChargeSkill->GetMaxGage());

		CharacterStatus = ECharacterStatus::ECS_Rotation;

		ChargeSkill->bKeyDown = true;
		ChargeSkill->bGageIncreasing = true;
		ChargeSkill->SetGageTimer();

		if (!HasAuthority())
		{
			ServerPlayMontage(ChargeSkill->SkillMontage, FName("Charging"), 0.2f);
		}
	}
}

void ABasicPlayer::ChargeAttack()
{
	if (ChargeSkill->bKeyDown)
	{
		PlayerController->HiddenSkillGage();

		float ChargeDamage = ChargeSkill->GetChargeDamage();
		if (!HasAuthority())
		{
			ServerPlayMontage(ChargeSkill->SkillMontage, FName("Attack"), 1.0f);
			ServerSpawnParticle(ChargeSkill->SkillParticles[0], GetActorLocation(), GetActorRotation());
			ServerPlaySound(ChargeSkill->SkillSounds[0]);
			ServerApplyDamageBasic(ChargeDamage, AttackRange);
		}

		ChargeSkill->bKeyDown = false;
		ChargeSkill->bGageIncreasing = false;
		ChargeSkill->ApplyCoolDown();
		ChargeSkill->SetCoolTimer();
	}
}

void ABasicPlayer::CastingKeyDown()
{
	if(CharacterStatus == ECharacterStatus::ECS_Normal && CastingSkill->GetCoolState() <= 0.0f)
	{
		CharacterStatus = ECharacterStatus::ECS_Attack;
		
		CastingSkill->bKeyDown = true;
	}
}

void ABasicPlayer::CastingStart()
{
	if(CastingSkill->bKeyDown)
	{
		PlayerController->VisibleSkillGage();

		SetCurrentMaxGage(CastingSkill->GetMaxGage());

		CastingSkill->bKeyDown = false;
		CastingSkill->bGageIncreasing = true;
		CastingSkill->SetGageTimer();

		if(!HasAuthority())
		{
			ServerPlayMontage(CastingSkill->SkillMontage, FName("Casting"), 0.3f);
			ServerSpawnParticle(CastingSkill->SkillParticles[0], GetActorLocation(), GetActorRotation());
		}

		GetWorldTimerManager().SetTimer(CastingTimer, this, &ABasicPlayer::CastingAttack, 0.5f, true, 0.0f);
	}
}

void ABasicPlayer::CastingAttack()
{
	if(CastingSkill->GetGage() >= CastingSkill->GetMaxGage())
	{
		PlayerController->HiddenSkillGage();

		GetWorldTimerManager().ClearTimer(CastingTimer);

		if (!HasAuthority())
		{
			ServerPlayMontage(CastingSkill->SkillMontage, FName("Attack"), 1.0f);
			ServerSpawnParticle(CastingSkill->SkillParticles[1], GetActorLocation(), GetActorRotation());
			ServerPlaySound(CastingSkill->SkillSounds[0]);
			ServerApplyDamageBasic(CastingSkill->SkillDamage, 0.0f);
		}

		CastingSkill->bGageIncreasing = false;
		CastingSkill->GageInit();
		CastingSkill->ApplyCoolDown();
		CastingSkill->SetCoolTimer();
	}
}

void ABasicPlayer::SetCurrentMaxGage(float SetGage)
{
	CurrentMaxGage = SetGage;
}

float ABasicPlayer::GetCurrentGage()
{
	return CurrentGage;
}

void ABasicPlayer::ServerPlayMontage_Implementation(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	MulticastPlayMontage(AnimMontage, SectionName, PlaySpeed);
}

bool ABasicPlayer::ServerPlayMontage_Validate(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	return true;
}

void ABasicPlayer::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	AnimInstance->Montage_Play(AnimMontage, PlaySpeed);
	AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
}

bool ABasicPlayer::MulticastPlayMontage_Validate(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	return true;
}

void ABasicPlayer::ServerSpawnParticle_Implementation(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation)
{
	MulticastSpawnParticle(ParticleSystem, SpawnLocation, SpawnRotation);
}

bool ABasicPlayer::ServerSpawnParticle_Validate(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation)
{
	return true;
}

void ABasicPlayer::MulticastSpawnParticle_Implementation(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, SpawnLocation, SpawnRotation);
}

bool ABasicPlayer::MulticastSpawnParticle_Validate(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation)
{
	return true;
}

void ABasicPlayer::ServerPlaySound_Implementation(USoundCue* SoundCue)
{
	MulticastPlaySound(SoundCue);
}

bool ABasicPlayer::ServerPlaySound_Validate(USoundCue* SoundCue)
{
	return true;
}

void ABasicPlayer::MulticastPlaySound_Implementation(USoundCue* SoundCue)
{
	UGameplayStatics::PlaySound2D(this, SoundCue);
}

bool ABasicPlayer::MulticastPlaySound_Validate(USoundCue* SoundCue)
{
	return true;
}

void ABasicPlayer::ServerApplyDamageBasic_Implementation(float Damage, float Distance)
{
	ApplyDamageBasic(Damage, Distance);
}

bool ABasicPlayer::ServerApplyDamageBasic_Validate(float Damage, float Distance)
{
	return true;
}

void ABasicPlayer::ApplyDamageBasic(float Damage, float Distance)
{
	FVector Range = GetAttackRangeFromFront(Distance);
	UGameplayStatics::ApplyRadialDamage(this, Damage, Range, AttackRadius, PlayerDamageType, AttackIgnoreActor, this, GetInstigatorController(), true);
	DrawDebugSphere(GetWorld(), Range, AttackRadius, 12, FColor::Red, false, 1.0f);
}

FVector ABasicPlayer::GetAttackRangeFromFront(float Distance)
{
	FRotator Rotation = GetActorRotation();
	FVector Location = Rotation.Vector();
	Location = GetActorLocation() + (Location * Distance);

	return Location;
}

void ABasicPlayer::OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && CharacterStatus != ECharacterStatus::ECS_Dead)
	{
		CharacterStatus = ECharacterStatus::ECS_Dead;
		GetMovementComponent()->StopMovementImmediately();
		DisableInput(PlayerController);
		if(HasAuthority())
		{
			MulticastPlayMontage(DeathMontage, FName("Death"), 0.5f);
		}
		SetLifeSpan(2.5f);
	}
}

void ABasicPlayer::ServerSetActorRotation_Implementation(FRotator NewRotation)
{
	MulticastSetActorRotation(NewRotation);
}

bool ABasicPlayer::ServerSetActorRotation_Validate(FRotator NewRotation)
{
	return true;
}

void ABasicPlayer::MulticastSetActorRotation_Implementation(FRotator NewRotation)
{
	SetActorRotation(NewRotation);
}

bool ABasicPlayer::MulticastSetActorRotation_Validate(FRotator NewRotation)
{
	return true;
}

float ABasicPlayer::GetHP()
{
	return HealthComponent->GetHealth();
}
