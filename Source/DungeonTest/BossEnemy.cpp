// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "BasicPlayerController.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABossEnemy::ABossEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->TeamNumber = ETeamNumber::ETN_Enemy;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Damage = 20.0f;
	AttackRadius = 50.0f;
	AttackRange = 150.0f;
	bAttacking = false;
	bDead = false;

	LaunchStrength = 1500;
	LaunchPitchAngle = 35.0f;
}

// Called when the game starts or when spawned
void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChanged.AddDynamic(this, &ABossEnemy::OnHealthChanged);

	AnimInstance = GetMesh()->GetAnimInstance();

	if(HasAuthority())
	{
		MulticastMontagePlay(BeginMontage, FName("Begin"), 1.0f);
	}

	if(!HasAuthority())
	{
		for(ABasicPlayerController* It : TActorRange<ABasicPlayerController>(GetWorld()))
		{
			It->SetBossEnemy(this);
			It->VisibleBossHealthBar();
		}
	}
}

// Called every frame
void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossEnemy::BasicAttack()
{
	if(!bAttacking)
	{
		bAttacking = true;

		if (HasAuthority())
		{
			int32 SectionNumber = FMath::RandRange(0, 2);
			switch (SectionNumber)
			{
			case 0:
				MulticastMontagePlay(AttackMontage, FName("AttackA"), 1.0f);
				break;
			case 1:
				MulticastMontagePlay(AttackMontage, FName("AttackC"), 1.0f);
				break;
			case 2:
				MulticastMontagePlay(AttackMontage, FName("ComboAttack"), 1.0f);
				break;
			default:
				MulticastMontagePlay(AttackMontage, FName("AttackA"), 1.0f);
				break;
			}
		}
	}
}

void ABossEnemy::ReadyLaunch()
{
	MulticastMontagePlay(LaunchMontage, FName("ReadyLaunch"), 1.0f);
}

void ABossEnemy::LaunchToTarget()
{
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;
	LaunchCharacter(LaunchVelocity, true, true);
}

void ABossEnemy::MulticastMontagePlay_Implementation(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	MontagePlay(AnimMontage, SectionName, PlaySpeed);
}

bool ABossEnemy::MulticastMontagePlay_Validate(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	return true;
}

void ABossEnemy::MontagePlay(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	AnimInstance->Montage_Play(AnimMontage, PlaySpeed);
	AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
}

void ABossEnemy::MulticastSoundPlay_Implementation(USoundCue* SoundCue)
{
	UGameplayStatics::PlaySound2D(this, SoundCue);
}

bool ABossEnemy::MulticastSoundPlay_Validate(USoundCue* SoundCue)
{
	return true;
}

void ABossEnemy::BPApplyDamage()
{
	if (HasAuthority())
	{
		ServerApplyDamage();
	}
}

void ABossEnemy::ServerApplyDamage_Implementation()
{
	FVector Location = GetAttackRangeFromFront();
	UGameplayStatics::ApplyRadialDamage(this, Damage, Location, AttackRadius, EnemyDamageType, AttackIgnoreActor, this, GetInstigatorController(), true);
}

bool ABossEnemy::ServerApplyDamage_Validate()
{
	return true;
}

FVector ABossEnemy::GetAttackRangeFromFront()
{
	FRotator Rotation = GetActorRotation();
	FVector Location = Rotation.Vector();
	Location = GetActorLocation() + (Location * AttackRange);

	return Location;
}

void ABossEnemy::EndAttack()
{
	bAttacking = false;
}

void ABossEnemy::OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDead)
	{
		bDead = true;
		GetMovementComponent()->StopMovementImmediately();
		DetachFromControllerPendingDestroy();

		MulticastMontagePlay(DeathMontage, FName("Death"), 1.0f);

		SetLifeSpan(2.5f);

		for (ABasicPlayerController* It : TActorRange<ABasicPlayerController>(GetWorld()))
		{
			It->HiddenBossHealthBar();
		}
	}
}

