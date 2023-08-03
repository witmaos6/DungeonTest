// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAnimInstance.h"

#include "BasicPlayer.h"

void UBasicAnimInstance::NativeInitializeAnimation()
{
	Pawn = TryGetPawnOwner();
	if(Pawn)
	{
		Character = Cast<ACharacter>(Pawn);
	}
}

void UBasicAnimInstance::UpdateAnimationProperties()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	
	if(Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		if (Character == nullptr)
		{
			Character = Cast<ACharacter>(Pawn);
		}
	}
}
