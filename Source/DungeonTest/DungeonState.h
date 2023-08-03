// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DungeonState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	FirstWave,

	SecondWave,

	BossWave,

	YouWin,

	GameOver
};

/**
 * 
 */
UCLASS()
class DUNGEONTEST_API ADungeonState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState PrevState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState CurrentWaveState, EWaveState PrevState);

public:
	void SetWaveState(EWaveState NewState);
};
