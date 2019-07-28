// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank_FR.h"

ATank_FR::ATank_FR()
{
	RightSprocket = CreateDefaultSubobject<UStaticMeshComponent>(FName("RightSprocket"));
	LeftSprocket = CreateDefaultSubobject<UStaticMeshComponent>(FName("LeftSprocket"));
	RightIdler = CreateDefaultSubobject<UStaticMeshComponent>(FName("RightIdler"));
	LeftIdler = CreateDefaultSubobject<UStaticMeshComponent>(FName("LeftIdler"));
	RightThread = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("RightThread"));
	LeftThread = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("LeftThread"));
	RightTrackSpline = CreateDefaultSubobject<USplineComponent>(FName("RightTrackSpline"));
	LeftTrackSpline = CreateDefaultSubobject<USplineComponent>(FName("LeftTrackSpline"));

	tail = CreateDefaultSubobject<UStaticMeshComponent>(FName("tail"));
	Suspensions = CreateDefaultSubobject<UStaticMeshComponent>(FName("Suspensions"));
	combParts = CreateDefaultSubobject<UStaticMeshComponent>(FName("combParts"));

	RightSprocket->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftSprocket->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	RightIdler->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftIdler->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	RightThread->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftThread->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	RightTrackSpline->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LeftTrackSpline->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	tail->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	Suspensions->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	combParts->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
}


void ATank_FR::BeginPlay()
{
	for (auto component : GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		if (component->ComponentTags.Contains(FName("Left")))
		{
			if (component->ComponentTags.Contains(FName("Suspension")))
			{
				LeftSuspensions.Add(Cast<UStaticMeshComponent>(component));
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
				RightSuspensions.Add(Cast<UStaticMeshComponent>(component));
			}
			else if (component->ComponentTags.Contains(FName("RoadWheel")))
			{
				RightRoadWheels.Add(Cast<UStaticMeshComponent>(component));
			}
		}
	}
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
