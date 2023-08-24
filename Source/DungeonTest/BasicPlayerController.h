// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasicPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONTEST_API ABasicPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> WBPMainUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UUserWidget* MainUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> WBPSkillGage;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	class USkillGageWidget* SkillGage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> WBPBossHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UUserWidget* BossHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	class ABossEnemy* BossEnemy;
	
protected:
	virtual void BeginPlay() override;

	void SetDisplay(TSubclassOf<UUserWidget>& WBPWidget, UUserWidget** Widget, ESlateVisibility DefaultSet);

public:
	void DisplayUI(UUserWidget* Widget, ESlateVisibility SetVisible);

	void VisibleSkillGage(float PlaySpeed, int32 NumLoops);

	void RemoveSkillGage();

	FORCEINLINE void SetBossEnemy(ABossEnemy* Boss) { BossEnemy = Boss; }

	void VisibleBossHealthBar();

	void HiddenBossHealthBar();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void PrintDamageText(const FVector& Location, float AppliedDamage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void BPPrintDamageText(const FVector& Location, float AppliedDamgage);
};
