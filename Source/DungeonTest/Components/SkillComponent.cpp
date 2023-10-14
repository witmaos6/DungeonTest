// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"

#include "MPComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CoolDown = 10.0f;
	CoolState = 0.0f;
	
	MaxGage = 10.0f;
	Gage = 0.0f;
	GageValue = 0.0f;
	GageIncreasingSpeed = 1.0f;
	GageLoops = 1;

	SkillDamage = 5.0f;

	bKeyDown = false;
	bGageIncreasing = false;

	RequireMana = 5.0f;
	AttackRadius = 300.0f;
	AttackDistance = 200.0f;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	GageValue = MaxGage / 5.0f;

	Owner = Cast<ACharacter>(GetOwner());
}

bool USkillComponent::IsAvailable()
{
	UMPComponent* OwnerMPComponent = Cast<UMPComponent>(Owner->GetComponentByClass(UMPComponent::StaticClass()));
	if(OwnerMPComponent)
	{
		return (CoolState <= 0.0f && OwnerMPComponent->GetMana() >= RequireMana);
	}
	return (CoolState <= 0.0f);
}

void USkillComponent::ApplyCoolDown()
{
	CoolState = CoolDown;

	if (Owner && CoolState >= 0.0f)
	{
		Owner->GetWorldTimerManager().SetTimer(CoolTimer, this, &USkillComponent::CoolDecrease, GetWorld()->GetDeltaSeconds(), true, 0.0f);
	}
}

void USkillComponent::CoolDecrease()
{
	if(CoolState > 0.0f)
	{
		CoolState -= GetWorld()->GetDeltaSeconds();
	}
	else
	{
		Owner->GetWorldTimerManager().ClearTimer(CoolTimer);
	}
}

void USkillComponent::SetGageTimer()
{
	if (Owner)
	{
		float IncreasingTerm = GetReverse(GageIncreasingSpeed * 5.0f);
		Owner->GetWorldTimerManager().SetTimer(GageTimer, this, &USkillComponent::GageIncrease, IncreasingTerm, true, 0.0f);
	}
}

void USkillComponent::GageIncrease()
{
	if (Gage < MaxGage && bGageIncreasing)
	{
		Gage += GageValue;
	}
	else
	{
		bGageIncreasing = false;
		Owner->GetWorldTimerManager().ClearTimer(GageTimer);
	}
}

float USkillComponent::GetChargeDamage()
{
	float ResultDamage = SkillDamage * Gage;
	if (Gage >= MaxGage)
	{
		ResultDamage *= 2.0f;
	}
	Gage = 0.0f;

	return FMath::RoundToFloat(ResultDamage);
}