// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayer.h"

#include "DungeonTest/BasicEnemy.h"
#include "BasicPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DungeonTest/Components/HealthComponent.h"
#include "DungeonTest/Components/MPComponent.h"
#include "DungeonTest/Components/SkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"

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

	MPComponent = CreateDefaultSubobject<UMPComponent>("MPComponent");

	AttackRadius = 300.0f;
	AttackRange = 200.0f;
	BasicDamage = 15.0f;
	bPermitAttack = true;

	CharacterStatus = ECharacterStatus::ECS_Begin;
}

// Called when the game starts or when spawned
void ABasicPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABasicPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasicPlayer::MoveRight);

	PlayerInputComponent->BindAxis("CameraTurn", this, &ABasicPlayer::AddControllerYawInput);
}

void ABasicPlayer::MoveForward(float Value)
{
	if (CharacterStatus == ECharacterStatus::ECS_Normal || CharacterStatus == ECharacterStatus::ECS_Move)
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
	else if (CharacterStatus == ECharacterStatus::ECS_Rotation)
	{
		if (!HasAuthority())
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Yaw += Value * 2;
			ServerSetActorRotation(Rotation);
		}
	}
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(AnimMontage, PlaySpeed);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
	}
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

void ABasicPlayer::ServerApplyDamageBasic_Implementation(float Damage, float Distance, float Radius)
{
	ApplyDamageBasic(Damage, Distance, Radius);
}

bool ABasicPlayer::ServerApplyDamageBasic_Validate(float Damage, float Distance, float Radius)
{
	return true;
}

void ABasicPlayer::ApplyDamageBasic(float Damage, float Distance, float Radius)
{
	FVector Range = GetAttackRangeFromFront(Distance);
	UGameplayStatics::ApplyRadialDamage(this, Damage, Range, Radius, PlayerDamageType, AttackIgnoreActor, this, GetInstigatorController(), true);
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

void ABasicPlayer::ServerLaunchCharacter_Implementation()
{
	MulticastLaunchCharacter();
}

bool ABasicPlayer::ServerLaunchCharacter_Validate()
{
	return true;
}

void ABasicPlayer::MulticastLaunchCharacter_Implementation()
{
	const FRotator Rotation = GetCharacterMovement()->GetLastUpdateRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	LaunchCharacter(Direction * 3000, true, false);
}

bool ABasicPlayer::MulticastLaunchCharacter_Validate()
{
	return true;
}

float ABasicPlayer::GetHP()
{
	return HealthComponent->GetHealth();
}
