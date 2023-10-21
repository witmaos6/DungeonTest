// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonTest/CharacterBase/BasicPlayer.h"
#include "Aurora.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONTEST_API AAurora : public ABasicPlayer
{
	GENERATED_BODY()
	
public:
	AAurora();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* BeginParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BeginMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation") // To do: �⺻ ���ݵ� ��ų�� ���� ���·� ��ȯ�ϴ� ���� ���� �� �غ��δ�.
	UAnimMontage* BasicAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* ChargeSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* CastingSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* EvadeSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* FreezeSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* BasicAttackSound;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void BasicAttack();

	void Charging();

	void ChargeAttack();

	void CastingKeyDown();

	void CastingStart();

	void CastingAttack();

	void FreezeAttack();

	void Evade();

	virtual void OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
