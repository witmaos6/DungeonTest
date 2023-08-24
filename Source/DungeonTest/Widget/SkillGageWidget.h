// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillGageWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DUNGEONTEST_API USkillGageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* GageBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* GageUpAnimation;

public:
	void GageUp(float PlaySpeed, int32 NumLoops);
};
