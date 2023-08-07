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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bGageIncreasing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float SkillDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float AttackRadius;

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

	float DeltaTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float CoolDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CoolState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float CoolValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float MaxGage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float Gage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float GageValue;

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

	void GageInit();

	float GetChargeDamage();

	FORCEINLINE float GetCoolState() { return CoolState; }

	FORCEINLINE float GetMaxGage() { return MaxGage; }

	FORCEINLINE float GetGage() { return Gage; }

	FORCEINLINE float GetRequireMana() { return RequireMana; }
};
