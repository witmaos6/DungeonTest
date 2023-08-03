// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayerController.h"

#include "Blueprint/UserWidget.h"

void ABasicPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority())
	{	
		SetDisplay(WBPMainUI, &MainUI, ESlateVisibility::Visible);

		SetDisplay(WBPSkillGage, &SkillGage, ESlateVisibility::Hidden);
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

void ABasicPlayerController::VisibleSkillGage()
{
	if(SkillGage)
	{
		SkillGage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, "Don't Assign SkillGage");
	}
}

void ABasicPlayerController::HiddenSkillGage()
{
	if(SkillGage)
	{
		SkillGage->SetVisibility(ESlateVisibility::Hidden);
	}
}
