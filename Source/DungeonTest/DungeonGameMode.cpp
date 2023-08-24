// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGameMode.h"

#include "DungeonState.h"
#include "BasicEnemy.h"
#include "EngineUtils.h"
#include "Components/HealthComponent.h"

ADungeonGameMode::ADungeonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ADungeonState::StaticClass();

	CurrentSpawnTime = 0;
	SpawnDelay = 5.0f;

	CurrentState = EWaveState::FirstWave;

	bSpawnBoss = false;

	NrOfSpawnEnemiesMin.Add(EWaveState::FirstWave, 4);
	NrOfSpawnEnemiesMin.Add(EWaveState::SecondWave, 15);
	NrOfSpawnEnemiesMin.Add(EWaveState::BossWave, 1);

	NrOfSpawnEnemiesMax.Add(EWaveState::FirstWave, 5);
	NrOfSpawnEnemiesMax.Add(EWaveState::SecondWave, 20);
	NrOfSpawnEnemiesMax.Add(EWaveState::BossWave, 3);

	NrOfSpawnTimes.Add(EWaveState::FirstWave, 4);
	NrOfSpawnTimes.Add(EWaveState::SecondWave, 6);
	NrOfSpawnTimes.Add(EWaveState::BossWave, 10);
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
	CurrentSpawnTime++;
	if(CurrentSpawnTime >= GetSpawnTime())
	{
		CurrentSpawnTime = 0;
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
	return NrOfSpawnTimes[CurrentState];
}

int32 ADungeonGameMode::GetNrOfSpawnEnemies()
{
	return FMath::RandRange(NrOfSpawnEnemiesMin[CurrentState], NrOfSpawnEnemiesMax[CurrentState]);
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
