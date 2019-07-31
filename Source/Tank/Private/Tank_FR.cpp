// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank_FR.h"
#include "Kismet/KismetMathLibrary.h"

ATank_FR::ATank_FR()
{
	RightSprocket = CreateDefaultSubobject<UStaticMeshComponent>(FName("RightSprocket"));
	LeftSprocket = CreateDefaultSubobject<UStaticMeshComponent>(FName("LeftSprocket"));
	RightIdler = CreateDefaultSubobject<UStaticMeshComponent>(FName("RightIdler"));
	LeftIdler = CreateDefaultSubobject<UStaticMeshComponent>(FName("LeftIdler"));

	Burrel = CreateDefaultSubobject<UStaticMeshComponent>(FName("Burrel"));
	Suspensions = CreateDefaultSubobject<UStaticMeshComponent>(FName("Suspensions"));
	combParts = CreateDefaultSubobject<UStaticMeshComponent>(FName("combParts"));

	RightSprocket->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftSprocket->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	RightIdler->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftIdler->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	Burrel->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	Suspensions->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	combParts->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);

	Turret->SetRelativeLocation(Body->GetSocketLocation(FName("Turret")));
	Turret->SetRelativeRotation(Body->GetSocketRotation(FName("Turret")));

	// 
	TrackMassKg = 400;
	SprocketMassKg = 65;
	AirDensity = 1.29;
	DragSurfaceArea = 10;
	DragCoefficient = 0.8;
	SprocketRadiusCm = 24;
	GearRatios = { 4.35, 0, 3.81, 1.93, 1 };
	DifferentialRatio = 3.5;
	TransmissionEfficiency = 0.9;
	MuXStatic = 1;
	MuYStatic = 0.85;
	MuXKinetic = 0.5;
	MuYKinetic = 0.45;
	RollingFrictionCoeffient = 0.02;
	BrakeForce = 30;
	TreadLength = 972.5;
	SplineCoordinatesRight = {
			FVector(230, 109, 94),
			FVector(253, 109, 65),
			FVector(230, 109, 33),
			FVector(175, 109, 5),
			FVector(113, 109, 5),
			FVector(54, 109, 5),
			FVector(-8, 109, 5),
			FVector(-70, 109, 5),
			FVector(-135, 109, 5),
			FVector(-193, 109, 45),
			FVector(-188, 109, 90),
			FVector(-135, 109, 85),
			FVector(-70, 109, 85),
			FVector(-8, 109, 85),
			FVector(54, 109, 85),
			FVector(113, 109, 85),
			FVector(175, 109, 85),
	};
	SplineCoordinatesLeft = {
			FVector(230, -109, 94),
			FVector(253, -109, 65),
			FVector(230, -109, 33),
			FVector(175, -109, 5),
			FVector(113, -109, 5),
			FVector(54, -109, 5),
			FVector(-8, -109, 5),
			FVector(-70, -109, 5),
			FVector(-135, -109, 5),
			FVector(-193, -109, 45),
			FVector(-188, -109, 90),
			FVector(-135, -109, 85),
			FVector(-70, -109, 85),
			FVector(-8, -109, 85),
			FVector(54, -109, 85),
			FVector(113, -109, 85),
			FVector(175, -109, 85),
	};
	SplineTangents = {
			FVector(55, 0, 0),
			FVector(0, 0, -30),
			FVector(-20, 0, -20),
			FVector(-80, 0, 0),
			FVector(0, 0, 0),
			FVector(-74.5, 0, 0),
			FVector(-67.5, 0, 0),
			FVector(-66.5, 0, 0),
			FVector(-50, 0, 0),
			FVector(-70, 0, 100),
			FVector(20, 0, 0),
			FVector(0, 0, 0),
			FVector(0, 0, 0),
			FVector(68.5, 0, 0),
			FVector(66.5, 0, 0),
			FVector(67.5, 0, 0),
			FVector(55, 0, 0)
	};
	TreadUVTiles = 20;
	TreadsOnSide = 90;
	TreadHalfThickness = 2;
	SuspensionSetUpRight = {
		FSuspensionSetUp(FVector(), FRotator(), 15, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 15, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30)
	};
	SuspensionSetUpLeft = {
		FSuspensionSetUp(FVector(), FRotator(), 10, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 30),
		FSuspensionSetUp(FVector(), FRotator(), 23, 32)
	};
	SleepVelocity = 5;
	SleepTimerSeconds = 2;
	AutoGearBox = true;
	GearUpShiftPrc = 0.9;
	GearDownShiftPrc = 0.05;
	EngineExtraPowerRatio = 3;

	RegisterSuspensionHandles();
	ConstructSuspension();	
}


void ATank_FR::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	BuildTrackSpline(RightTrackSpline, LeftTrackSpline, RightTreads, LeftTreads);
}

void ATank_FR::BeginPlay()
{
	Super::BeginPlay();
}

void ATank_FR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATank_FR::AnimateWheels()
{
	for (size_t i = 0; i < SuspensionsInternalRight.Num(); i++)
	{
		PositionAndAnimateDriveWheels(RightRoadWheels[i], SuspensionsInternalRight[i], i, ESide::Right, false);
	}

	for (size_t i = 0; i < SuspensionsInternalLeft.Num(); i++)
	{
		PositionAndAnimateDriveWheels(LeftRoadWheels[i], SuspensionsInternalLeft[i], i, ESide::Left, false);
	}

	AnimateSprocketOrIdler(RightSprocket, TrackRightAngularVelocity, false);
	AnimateSprocketOrIdler(LeftSprocket, TrackLeftAngularVelocity, true);
	AnimateSprocketOrIdler(RightIdler, TrackRightAngularVelocity, false);
	AnimateSprocketOrIdler(LeftIdler, TrackLeftAngularVelocity, true);
}

void ATank_FR::AnimateTreadsSpline()
{
	for (size_t i = 0; i < RightRoadWheels.Num(); i++)
	{
		AnimateTreadsSplineControlPoints(
			RightRoadWheels[i],
			RightTrackSpline,
			3 + i,
			16 - i,
			SplineCoordinatesRight,
			SuspensionSetUpRight,
			i
		);
	}

	for (size_t i = 0; i < LeftRoadWheels.Num(); i++)
	{
		AnimateTreadsSplineControlPoints(
			LeftRoadWheels[i],
			LeftTrackSpline,
			3 + i,
			16 - i,
			SplineCoordinatesLeft,
			SuspensionSetUpLeft,
			i
		);
	}
}

void ATank_FR::RegisterSuspensionHandles()
{
	for (auto component : GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		if (component->ComponentTags.Contains(FName("Left")))
		{
			if (component->ComponentTags.Contains(FName("Suspension")))
			{
				SuspensionHandleLeft.Add(Cast<UStaticMeshComponent>(component));
			}
			else if (component->ComponentTags.Contains(FName("RoadWheel")))
			{
				LeftRoadWheels.Add(Cast<UStaticMeshComponent>(component));
			}
		}
		else if (component->ComponentTags.Contains(FName("Right")))
		{
			if (component->ComponentTags.Contains(FName("Suspension")))
			{
				SuspensionHandleRight.Add(Cast<UStaticMeshComponent>(component));
			}
			else if (component->ComponentTags.Contains(FName("RoadWheel")))
			{
				RightRoadWheels.Add(Cast<UStaticMeshComponent>(component));
			}
		}
	}
}
