// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonGameMode.generated.h"


enum class EWaveState : uint8;
/**
 * 
 */

UCLASS()
class DUNGEONTEST_API ADungeonGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	int32 SpawnTime;

	FTimerHandle SpawnTimer;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float SpawnDelay;

	EWaveState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	bool bSpawnBoss;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	TSubclassOf<AActor> BossSpawnLocation;

public:
	ADungeonGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	bool GetSpawnBoss() { return bSpawnBoss; }

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewEnemy();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBoss();

	void StartWave();

	int32 GetSpawnTime();

	int32 GetNrOfSpawnEnemies();

	bool bRemainEnemies();

	void SetWaveState(EWaveState NewState);
};
