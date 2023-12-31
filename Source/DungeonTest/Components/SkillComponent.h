// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class USoundCue;

UCLASS( ClassGroup=(Dungeon), meta=(BlueprintSpawnableComponent) )
class DUNGEONTEST_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bKeyDown;

	bool bGageIncreasing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float SkillDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float AttackDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TArray<USoundCue*> SkillSounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SkillMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<float> AnimPlaySpeeds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particle")
	TArray<UParticleSystem*> SkillParticles;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float CoolDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CoolState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float MaxGage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float Gage;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float GageIncreasingSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	float GageValue;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	int32 GageLoops;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float RequireMana;

	ACharacter* Owner;

	FTimerHandle CoolTimer;

	FTimerHandle GageTimer;

public:
	bool IsAvailable();

	void ApplyCoolDown();

	void CoolDecrease();

	void SetGageTimer();

	void GageIncrease();

	float GetChargeDamage();

	FORCEINLINE float GetRequireMana() { return RequireMana; }

	FORCEINLINE float GetCoolState() { return CoolState; }

	FORCEINLINE float GetMaxGage() { return MaxGage; }

	FORCEINLINE float GetGage() { return Gage; }

	FORCEINLINE float GetGageIncreasingSpeed() { return GageIncreasingSpeed; }

	FORCEINLINE int32 GetGageLoops() { return GageLoops; }

	FORCEINLINE float GetReverse(float Value) { return 1.f / Value; }

	FORCEINLINE float GetExcuteTime(float AddValue) { return GetReverse(GageIncreasingSpeed) * GageLoops + AddValue; }
};
