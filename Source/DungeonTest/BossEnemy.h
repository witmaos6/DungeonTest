// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossEnemy.generated.h"

UCLASS()
class DUNGEONTEST_API ABossEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<UDamageType> EnemyDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BeginMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* LaunchMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	TArray<AActor*> AttackIgnoreActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundCue* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchPitchAngle;

	bool bDead;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void BasicAttack();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void ReadyLaunch();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void LaunchToTarget();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastMontagePlay(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed);

	void MontagePlay(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void MulticastSoundPlay(USoundCue* SoundCue);

	UFUNCTION(BlueprintCallable)
	void BPApplyDamage();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyDamage();

	FVector GetAttackRangeFromFront();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
