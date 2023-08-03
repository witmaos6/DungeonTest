// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGameMode.h"

#include "BasicEnemy.h"
#include "DungeonState.h"
#include "EngineUtils.h"
#include "HealthComponent.h"

ADungeonGameMode::ADungeonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ADungeonState::StaticClass();

	SpawnTime = 0;
	SpawnDelay = 5.0f;

	CurrentState = EWaveState::FirstWave;

	bSpawnBoss = false;
}

void ADungeonGameMode::StartPlay()
{
	Super::StartPlay();

	SetWaveState(EWaveState::FirstWave);

	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ADungeonGameMode::StartWave, SpawnDelay, true, 5.0f);
}

void ADungeonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!bRemainEnemies() && !GetWorldTimerManager().IsTimerActive(SpawnTimer))
	{
		if(CurrentState == EWaveState::FirstWave)
		{
			CurrentState = EWaveState::SecondWave;
			SetWaveState(EWaveState::SecondWave);

			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ADungeonGameMode::StartWave, SpawnDelay, true, 0.0f);
		}
		else if(CurrentState == EWaveState::SecondWave)
		{
			CurrentState = EWaveState::BossWave;
			SetWaveState(EWaveState::BossWave);

			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ADungeonGameMode::StartWave, SpawnDelay, true, 0.0f);

			if(!bSpawnBoss)
			{
				SpawnBoss();
			}
		}
	}
}

void ADungeonGameMode::StartWave()
{
	SpawnTime++;
	if(SpawnTime >= GetSpawnTime())
	{
		SpawnTime = 0;
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		return;
	}

	int32 NrOfSpawnEnemies = GetNrOfSpawnEnemies();
	for (int i = 0; i < NrOfSpawnEnemies; i++)
	{
		SpawnNewEnemy();
	}
}

int32 ADungeonGameMode::GetSpawnTime()
{
	int32 Result = 1;

	if (CurrentState == EWaveState::FirstWave)
	{
		Result = 4;
	}
	else if (CurrentState == EWaveState::SecondWave)
	{
		Result = 6;
	}
	else if(CurrentState == EWaveState::BossWave)
	{
		Result = 10;
	}

	return Result;
}

int32 ADungeonGameMode::GetNrOfSpawnEnemies()
{
	int32 Result = 0;

	if(CurrentState == EWaveState::FirstWave)
	{
		Result = FMath::RandRange(4, 5);
	}
	else if(CurrentState == EWaveState::SecondWave)
	{
		Result = FMath::RandRange(15, 20);
	}
	else if(CurrentState == EWaveState::BossWave)
	{
		Result = FMath::RandRange(1, 3);
	}

	return Result;
}

bool ADungeonGameMode::bRemainEnemies()
{
	for (ABasicEnemy* It : TActorRange<ABasicEnemy>(GetWorld()))
	{
		UHealthComponent* HealthComponent = Cast<UHealthComponent>(It->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			return true;
		}
	}
	return false;
}

void ADungeonGameMode::SetWaveState(EWaveState NewState)
{
	ADungeonState* DS = GetGameState<ADungeonState>();

	if(ensureAlways(DS))
	{
		DS->SetWaveState(NewState);
	}
}
