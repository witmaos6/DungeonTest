// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGageWidget.h"

#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

void USkillGageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USkillGageWidget::GageUp(float PlaySpeed, int32 NumLoops)
{
	PlayAnimation(GageUpAnimation, 0.0f, NumLoops, EUMGSequencePlayMode::Forward, PlaySpeed);
}