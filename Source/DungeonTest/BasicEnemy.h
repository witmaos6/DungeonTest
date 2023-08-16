// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyStatus : uint8
{
	EES_Normal UMETA(DisplayName = "Normal"),
	EES_Move UMETA(DisplayName = "Move"),
	EES_Attack UMETA(DisplayName = "Attack"),
	EES_Stiffen UMETA(DisplayName = "Stiffen"),
	EES_Dead UMETA(DisplayName = "Dead")
};

class USoundCue;

UCLASS()
class DUNGEONTEST_API ABasicEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle SetAIControllerTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EEnemyStatus EnemyStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float StopDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class ABasicPlayer* AttackTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float ValidAttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BasicAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DamagedMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<UDamageType> EnemyDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	bool bAttacking;

	TArray<AActor*> AttackIgnoreActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTarget();

protected:
	void SetAIController();

	ABasicPlayer* GetNextTarget();

	void MoveToTarget();

	void BasicAttack();

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

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void PrintDamageText(float AppliedDamage);
};
