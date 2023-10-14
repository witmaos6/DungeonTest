// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Widget/SkillGageWidget.h"

void ABasicPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		SetDisplay(WBPMainUI, &MainUI, ESlateVisibility::Visible);

		SetDisplay(WBPBossHealthBar, &BossHealthBar, ESlateVisibility::Hidden);
	}
}

void ABasicPlayerController::SetDisplay(TSubclassOf<UUserWidget>& WBPWidget, UUserWidget** Widget, ESlateVisibility DefaultSet)
{
	if (WBPWidget)
	{
		*Widget = CreateWidget<UUserWidget>(this, WBPWidget);

		UUserWidget* CurrWidget = *Widget;
		CurrWidget->AddToViewport();
		CurrWidget->SetVisibility(DefaultSet);
	}
}

void ABasicPlayerController::DisplayUI(UUserWidget* Widget, ESlateVisibility SetVisible)
{
	Widget->SetVisibility(SetVisible);
}

void ABasicPlayerController::VisibleSkillGage(float PlaySpeed, int32 NumLoops)
{
	if (!HasAuthority())
	{
		SkillGage = CreateWidget<USkillGageWidget>(this, WBPSkillGage);
		SkillGage->AddToViewport();
		SkillGage->SetVisibility(ESlateVisibility::Visible);
		
		SkillGage->GageUp(PlaySpeed, NumLoops);
	}
}

void ABasicPlayerController::RemoveSkillGage()
{
	if (SkillGage)
	{
		SkillGage->RemoveFromParent();
	}
}

void ABasicPlayerController::VisibleBossHealthBar()
{
	if(BossHealthBar)
	{
		BossHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetDisplay(WBPBossHealthBar, &BossHealthBar, ESlateVisibility::Visible);
	}
}

void ABasicPlayerController::HiddenBossHealthBar()
{
	if(BossHealthBar)
	{
		BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasicPlayerController::PrintDamageText_Implementation(const FVector& Location, float AppliedDamage)
{
	if (!HasAuthority())
	{
		BPPrintDamageText(Location, AppliedDamage);
	}
}

bool ABasicPlayerController::PrintDamageText_Validate(const FVector& Location, float AppliedDamage)
{
	return true;
}
