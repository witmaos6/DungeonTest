// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonState.h"

#include "Net/UnrealNetwork.h"

void ADungeonState::OnRep_WaveState(EWaveState PrevState)
{
	WaveStateChanged(WaveState, PrevState);
}

void ADungeonState::SetWaveState(EWaveState NewState)
{
	if(HasAuthority())
	{
		EWaveState PrevState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(PrevState);
	}
}

void ADungeonState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonState, WaveState);
}
