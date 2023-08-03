// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"

#include "BasicPlayer.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CoolDown = 10.0f;
	CoolState = 0.0f;
	CoolValue = 1.0f;

	MaxGage = 10.0f;
	Gage = 0.0f;
	GageValue = 10.0f;

	SkillDamage = 5.0f;

	bKeyDown = false;
	bGageIncreasing = false;
}

// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	DeltaTime = GetWorld()->GetDeltaSeconds();

	Owner = Cast<ABasicPlayer>(GetOwner());
}

void USkillComponent::ApplyCoolDown()
{
	CoolState = CoolDown;
}

void USkillComponent::SetCoolTimer()
{
	if(Owner && CoolState >= 0.0f)
	{
		Owner->GetWorldTimerManager().SetTimer(CoolTimer, this, &USkillComponent::CoolDecrease, DeltaTime, true, 0.0f);
	}
}

void USkillComponent::CoolDecrease()
{
	if(CoolState > 0.0f)
	{
		CoolState -= CoolValue * DeltaTime;
	}
	else
	{
		Owner->GetWorldTimerManager().ClearTimer(CoolTimer);
	}
}

void USkillComponent::SetGageTimer()
{
	if(Owner)
	{
		Owner->GetWorldTimerManager().SetTimer(GageTimer, this, &USkillComponent::GageIncrease, DeltaTime, true, 0.0f);
	}
}

void USkillComponent::GageIncrease()
{
	if(Gage < MaxGage && bGageIncreasing)
	{
		Gage += GageValue * DeltaTime;
	}
	else
	{
		bGageIncreasing = false;
		Owner->GetWorldTimerManager().ClearTimer(GageTimer);
	}
}

void USkillComponent::GageInit()
{
	Gage = 0.0f;
}

float USkillComponent::GetChargeDamage()
{
	float ResultDamage = SkillDamage * Gage;
	if (Gage >= MaxGage)
	{
		ResultDamage *= 2.0f;
	}
	GageInit();
	return FMath::RoundToFloat(ResultDamage);
}
