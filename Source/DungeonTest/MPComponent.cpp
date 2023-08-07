// Fill out your copyright notice in the Description page of Project Settings.


#include "MPComponent.h"

// Sets default values for this component's properties
UMPComponent::UMPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;

	MaxMana = 100.0f;
	ManaWeight = 10.0f;
}


// Called when the game starts
void UMPComponent::BeginPlay()
{
	Super::BeginPlay();

	Mana = MaxMana;
}


// Called every frame
void UMPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(Mana < MaxMana)
	{
		Mana += ManaWeight;

		if(Mana > MaxMana)
		{
			Mana = MaxMana;
		}
	}
}

void UMPComponent::ReduceMana(float Cost)
{
	if(Mana >= Cost)
	{
		Mana -= Cost;
	}
}
