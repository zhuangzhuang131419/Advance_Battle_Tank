// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneralTypes.generated.h"

UENUM(BlueprintType)
enum class ESide : uint8
{
	Left,
	Right
};

UENUM(BlueprintType)
enum class EAutoForwardInput : uint8
{
	Forward,
	Neutral,
	Backward
};

USTRUCT(BlueprintType)
struct FSuspensionInternalProcessing
{
	GENERATED_USTRUCT_BODY()

	FVector RootLoc;
	FRotator RootRot;
	float Length = 100;
	float Radius = 100;
	float Stiffness = 0.5;
	float Damping = 0.5;
	float PreviousLength;
	FVector SuspensionForce;
	FVector WheelCollisionLocation;
	FVector WheelCollisionNormal;
	bool Engaged = false;
	EPhysicalSurface HitMaterial;

	FSuspensionInternalProcessing(FVector RootLoc, FRotator RootRot, float Length, float Radius, float Stiffness, float Damping, float PreviousLength)
	{
		this->RootLoc = RootLoc;
		this->RootRot = RootRot;
		this->Length = Length;
		this->Radius = Radius;
		this->Stiffness = Stiffness;
		this->Damping = Damping;
		this->PreviousLength = PreviousLength;
	}
};

USTRUCT(BlueprintType)
struct FSuspensionSetUp
{
	GENERATED_USTRUCT_BODY()

	FVector RootLoc;
	FRotator RootRot;
	float MaxLength = 23;
	float CollisionRadius = 34;
	float StiffnessForce = 4000000;
	float DampingForce = 4000;
};

/**
 * 
 */
UCLASS()
class BATTLE_TANK_API UGeneralTypes
{
	
	
};
