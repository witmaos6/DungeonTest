// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultHealth = 100.0f;
	TeamNumber = ETeamNumber::ETN_Default;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole() == ROLE_Authority)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}
	
	Health = DefaultHealth;
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (DamageCauser == DamagedActor || IsFriendly(DamageCauser, DamagedActor))
	{
		return;
	}
	if (Damage <= 0.0f)
	{
		return;
	}

	Health -= Damage;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

float UHealthComponent::GetHealth() const
{
	return Health;
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if(ActorA == nullptr && ActorB == nullptr)
	{
		return false;
	}

	UHealthComponent* HealthCompA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent* HealthCompB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return false;
	}

	return (HealthCompA->TeamNumber == HealthCompB->TeamNumber);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}
