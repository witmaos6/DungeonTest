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

	UPROPERTY(EditDefaultsOnly, Category = "Sound") // To do: �̰͵� SkillParticlesó�� �迭�� ��ȯ
	USoundCue* SkillSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SkillMontage;

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

	class ABasicPlayer* Owner;

	FTimerHandle CoolTimer;

	FTimerHandle GageTimer;

public:
	void ApplyCoolDown();

	void SetCoolTimer();

	void CoolDecrease();

	void SetGageTimer();

	void GageIncrease();

	void GageInit();

	float GetChargeDamage();

	FORCEINLINE float GetCoolState() { return CoolState; }

	FORCEINLINE float GetMaxGage() { return MaxGage; }

	FORCEINLINE float GetGage() { return Gage; }
};