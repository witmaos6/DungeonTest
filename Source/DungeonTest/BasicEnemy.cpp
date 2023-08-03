// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemy.h"

#include "AIController.h"
#include "BasicPlayer.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABasicEnemy::ABasicEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->TeamNumber = ETeamNumber::ETN_Enemy;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	StopDistance = 20.0f;
	ValidAttackRadius = 250.0f;

	Damage = 10.0f;
	AttackRadius = 50.0f;
	AttackDelay = 1.0f;
	AttackRange = 150.0f;
	bAttacking = false;
}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(SetAIControllerTimer, this, &ABasicEnemy::SetAIController, 1.0f, true, 0.0f);

	HealthComponent->OnHealthChanged.AddDynamic(this, &ABasicEnemy::OnHealthChanged);

	if(HasAuthority())
	{
		AttackTarget = GetNextTarget();
	}
	
	EnemyStatus = EEnemyStatus::EES_Normal;

	AttackIgnoreActor.Add(this);

	AnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void ABasicEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && EnemyStatus != EEnemyStatus::EES_Dead && EnemyStatus != EEnemyStatus::EES_Stiffen)
	{
		if (AttackTarget && AttackTarget->GetHP() > 0.0f)
		{
			if (EnemyStatus == EEnemyStatus::EES_Normal)
			{
				MoveToTarget();
			}
			else if (EnemyStatus == EEnemyStatus::EES_Move)
			{
				const float DistanceToTarget = (AttackTarget->GetActorLocation() - GetActorLocation()).Size();
				if (DistanceToTarget <= ValidAttackRadius)
				{
					EnemyStatus = EEnemyStatus::EES_Attack;
				}
				else
				{
					EnemyStatus = EEnemyStatus::EES_Normal;
				}
			}
			else if (EnemyStatus == EEnemyStatus::EES_Attack)
			{
				if (!bAttacking)
				{
					BasicAttack();
				}
			}
		}
		else if (AttackTarget == nullptr || AttackTarget->GetHP() <= 0.0f)
		{
			SetTarget();
		}
	}
}

void ABasicEnemy::SetTarget()
{
	AttackTarget = GetNextTarget();
}

void ABasicEnemy::SetAIController()
{
	if(AIController == nullptr)
	{
		AIController = Cast<AAIController>(GetController());
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SetAIControllerTimer);
	}
}

ABasicPlayer* ABasicEnemy::GetNextTarget()
{
	ABasicPlayer* NearestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;
	for(ABasicPlayer* It : TActorRange<ABasicPlayer>(GetWorld()))
	{
		UHealthComponent* CharacterHealthComponent = Cast<UHealthComponent>(It->GetComponentByClass(UHealthComponent::StaticClass()));

		if(CharacterHealthComponent && CharacterHealthComponent->GetHealth() > 0.0f)
		{
			float Distance = (It->GetActorLocation() - GetActorLocation()).Size();

			if (Distance < NearestTargetDistance)
			{
				NearestTarget = It;
				NearestTargetDistance = Distance;
			}
		}
	}

	if(NearestTarget)
	{
		return NearestTarget;
	}
	return nullptr;
}

void ABasicEnemy::MoveToTarget()
{
	EnemyStatus = EEnemyStatus::EES_Move;

	if(AttackTarget && AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(AttackTarget);
		MoveRequest.SetAcceptanceRadius(StopDistance);
		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void ABasicEnemy::BasicAttack()
{
	bAttacking = true;
	if(AIController)
	{
		AIController->StopMovement();
	}

	if(HasAuthority())
	{
		MulticastMontagePlay(BasicAttackMontage, FName("BasicAttack"), 1.0f);
	}
}

void ABasicEnemy::MulticastMontagePlay_Implementation(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	MontagePlay(AnimMontage, SectionName, PlaySpeed);
}

bool ABasicEnemy::MulticastMontagePlay_Validate(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	return true;
}

void ABasicEnemy::MontagePlay(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed)
{
	AnimInstance->Montage_Play(AnimMontage, PlaySpeed);
	AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
}

void ABasicEnemy::MulticastSoundPlay_Implementation(USoundCue* SoundCue)
{
	UGameplayStatics::PlaySound2D(this, SoundCue);
}

bool ABasicEnemy::MulticastSoundPlay_Validate(USoundCue* SoundCue)
{
	return true;
}

void ABasicEnemy::BPApplyDamage()
{	
	if(HasAuthority())
	{
		ServerApplyDamage();
	}
}

void ABasicEnemy::ServerApplyDamage_Implementation()
{
	FVector Location = GetAttackRangeFromFront();
	UGameplayStatics::ApplyRadialDamage(this, Damage, Location, AttackRadius, EnemyDamageType, AttackIgnoreActor, this, GetInstigatorController(), true);
}

bool ABasicEnemy::ServerApplyDamage_Validate()
{
	return true;
}

FVector ABasicEnemy::GetAttackRangeFromFront()
{
	FRotator Rotation = GetActorRotation();
	FVector Location = Rotation.Vector();
	Location = GetActorLocation() + (Location * AttackRange);

	return Location;
}

void ABasicEnemy::EndAttack()
{
	bAttacking = false;
	EnemyStatus = EEnemyStatus::EES_Normal;
}

void ABasicEnemy::OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && EnemyStatus != EEnemyStatus::EES_Dead)
	{
		EnemyStatus = EEnemyStatus::EES_Dead;

		GetMovementComponent()->StopMovementImmediately();
		DetachFromControllerPendingDestroy();

		MulticastMontagePlay(DamagedMontage, FName("Death"), 1.0f);

		SetLifeSpan(2.3f);
	}
	else if(Health > 0.0f)
	{
		EnemyStatus = EEnemyStatus::EES_Stiffen;

		MulticastMontagePlay(DamagedMontage, FName("Stiffen"), 0.5f);
	}
}