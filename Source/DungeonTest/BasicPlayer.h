// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicPlayer.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
	ECS_Begin UMETA(DisplayName = "Begin"),
	ECS_Normal UMETA(DisplayName = "Normal"),
	ECS_Move UMETA(DisplayName = "Move"),
	ECS_Rotation UMETA(DisplayName = "Rotation"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Evade UMETA(DisplayName = "Evade"),

	ECS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EAttackStatus : uint8
{
	EAS_Normal UMETA(DisplayName = "Normal"),
	EAS_FirstAttack UMETA(DisplayName = "FirstAttack"),
	EAS_SecondAttack UMETA(DisplayName = "SecondAttack"),
	EAS_ThirdAttack UMETA(DisplayName = "ThirdAttack")
};

class USkillComponent;
class USoundCue;

UCLASS()
class DUNGEONTEST_API ABasicPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMPComponent* MPComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ECharacterStatus CharacterStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EAttackStatus AttackStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BeginMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UParticleSystem* BeginParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation") // To do: 기본 공격도 스킬과 같은 형태로 전환하는 것이 좋을 듯 해보인다.
	UAnimMontage* BasicAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bPermitAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	TArray<AActor*> AttackIgnoreActor; // ApplyRadialDamage에 한정된 멤버 변수이므로 HealthComponent에 IsFriend와 같은 함수를 추가하는 방향으로 가는 것이 좋을 것 같다.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class ABasicPlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<UDamageType> PlayerDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float BasicDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* ChargeSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* CastingSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* EvadeSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillComponent* FreezeSkill;

	FTimerHandle CastingTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CurrentMaxGage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CurrentGage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* BasicAttackSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void MoveForward(float Value);

	void MoveRight(float Value);

	void BasicAttack();

	void Charging();

	UFUNCTION(BlueprintCallable)
	void ChargeAttack();

	void CastingKeyDown();

	void CastingStart();

	void CastingAttack();

	void SetCurrentMaxGage(float SetGage);

	float GetCurrentGage();

	void FreezeAttack();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerPlayMontage(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, FName SectionName, float PlaySpeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnParticle(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastSpawnParticle(UParticleSystem* ParticleSystem, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlaySound(USoundCue* SoundCue);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlaySound(USoundCue* SoundCue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyDamageBasic(float Damage, float Distance, float Radius);

	void ApplyDamageBasic(float Damage, float Distance, float Radius);

	FVector GetAttackRangeFromFront(float Distance);

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwnerHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetActorRotation(FRotator NewRotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastSetActorRotation(FRotator NewRotation);

	void Evade();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLaunchCharacter();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastLaunchCharacter();

public:
	float GetHP();
};
