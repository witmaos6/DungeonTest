// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONTEST_API UMPComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MPComponent")
	float MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MPComponent")
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MPComponent")
	float ManaWeight;

public:	
	// Sets default values for this component's properties
	UMPComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReduceMana(float Cost);

	FORCEINLINE float GetMana() { return Mana; }
};
