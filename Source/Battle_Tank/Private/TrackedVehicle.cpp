// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackedVehicle.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATrackedVehicle::ATrackedVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName("Body"));
	COM = CreateDefaultSubobject<UArrowComponent>(FName("COM"));
	ThreadR = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ThreadR"));
	ThreadL = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ThreadL"));
	WheelSweep = CreateDefaultSubobject<UStaticMeshComponent>(FName("WheelSweep"));
	Turrent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Turrent"));
	MainCam = CreateDefaultSubobject<USpringArmComponent>(FName("MainCam"));
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Cannon = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Cannon"));
	TurrentCam = CreateDefaultSubobject<USpringArmComponent>(FName("TurrentCam"));
	Front = CreateDefaultSubobject<USpringArmComponent>(FName("Front"));
	LookRight = CreateDefaultSubobject<USpringArmComponent>(FName("LookRight"));
	LookLeft = CreateDefaultSubobject<USpringArmComponent>(FName("LookLeft"));

	COM->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	ThreadR->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	ThreadL->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	WheelSweep->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	Turrent->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	MainCam->AttachToComponent(Turrent, FAttachmentTransformRules::KeepWorldTransform);
	Camera->AttachToComponent(MainCam, FAttachmentTransformRules::KeepWorldTransform);
	Cannon->AttachToComponent(Turrent, FAttachmentTransformRules::KeepWorldTransform);
	TurrentCam->AttachToComponent(Cannon, FAttachmentTransformRules::KeepWorldTransform);
	Front->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LookRight->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);
	LookLeft->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform);

	PreCalculateMomentOfInteria();

}

// Called when the game starts or when spawned
void ATrackedVehicle::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ATrackedVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PutToSleep())
	{
		UpdateThrottle();
		UpdateWheelsVelocity();

		// TODO
		// Animate Threads Material
		// Animate Threads Spline
		// Animate Threads Instanced Mesh
		UpdateAxlsVelocity();
		CalculateEngineAndUpdateDrive();
		for (size_t i = 0; i < SuspensionsInternalRight.Num(); i++)
		{
			CheckWheelCollision(i, SuspensionsInternalRight, ESide::Right);
		}

		for (size_t i = 0; i < SuspensionsInternalLeft.Num(); i++)
		{
			CheckWheelCollision(i, SuspensionsInternalLeft, ESide::Left);
		}

		CountFrictionContactPoint(SuspensionsInternalRight);
		CountFrictionContactPoint(SuspensionsInternalLeft);

		ApplyDriveForceAndGetFrictionForceOnSide(SuspensionsInternalRight, DriveRightForce, TrackRightLinearVelocity, TrackFrictionTorqueRight, TrackRollingFrictionTorqueRight);
		ApplyDriveForceAndGetFrictionForceOnSide(SuspensionsInternalLeft, DriveLeftForce, TrackLeftLinearVelocity, TrackFrictionTorqueLeft, TrackRollingFrictionTorqueLeft);

		SpawnDust(SuspensionsInternalRight, TrackRightLinearVelocity);
		SpawnDust(SuspensionsInternalLeft, TrackLeftLinearVelocity);

		TotalNumFrictionPoints = 0;
	}
}

// Called to bind functionality to input
void ATrackedVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATrackedVehicle::PreCalculateMomentOfInteria()
{
	// 计算转动惯量
	MomentInertia = (SprocketMassKg * 0.5 + TrackMassKg) * SprocketRadiusCm * SprocketRadiusCm;
}

void ATrackedVehicle::ConstructSuspension()
{
	//SuspensionSetUpRight.Add(&FSuspensionSetUp());
	//SuspensionSetUpRight.Add(&FSuspensionSetUp());

	for (size_t i = 0; i < SuspensionHandleRight.Num(); i++)
	{
		SuspensionsInternalRight.Add(FSuspensionInternalProcessing(
			SuspensionHandleRight[i]->GetRelativeTransform().GetLocation(),
			SuspensionHandleRight[i]->GetRelativeTransform().GetRotation().Rotator(),
			SuspensionSetUpRight[i].MaxLength,
			SuspensionSetUpRight[i].CollisionRadius,
			SuspensionSetUpRight[i].StiffnessForce,
			SuspensionSetUpRight[i].DampingForce
		));
	}

	for (size_t i = 0; i < SuspensionHandleLeft.Num(); i++)
	{
		SuspensionsInternalLeft.Add(FSuspensionInternalProcessing(
			SuspensionHandleLeft[i]->GetRelativeTransform().GetLocation(),
			SuspensionHandleLeft[i]->GetRelativeTransform().GetRotation().Rotator(),
			SuspensionSetUpLeft[i].MaxLength,
			SuspensionSetUpLeft[i].CollisionRadius,
			SuspensionSetUpLeft[i].StiffnessForce,
			SuspensionSetUpLeft[i].DampingForce
		));
	}
}

void ATrackedVehicle::VisualizeCenterOfMass()
{
	COM->SetWorldLocation(Body->GetCenterOfMass());
}

void ATrackedVehicle::AddWheelForce(UPrimitiveComponent* Wheel, FVector Force)
{
	WheelLoc = Wheel;
	RelativeTransform = FTransform(GetActorRotation(), WheelLoc->RelativeLocation, WheelLoc->RelativeScale3D);
	FVector start = WheelLoc->GetComponentLocation();
	FVector end = start + UKismetMathLibrary::TransformDirection(RelativeTransform, FVector(0, 0, -1)) * 110;
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(
		hitResult,
		start,
		end,
		ECollisionChannel::ECC_Visibility
	);

	if (hitResult.bBlockingHit)
	{
		FVector transformedForce = UKismetMathLibrary::TransformDirection(RelativeTransform, Force);
		WheelLoc->AddForce(transformedForce);
	}
	
}

void ATrackedVehicle::AddWheelForceImproved(UPrimitiveComponent* Wheel, FVector Force, FHitResult HitStruct, UPhysicsConstraintComponent* Suspension)
{
	WheelLoc = Wheel;
	RelativeTransform = FTransform(GetActorRotation(), WheelLoc->RelativeLocation, WheelLoc->RelativeScale3D);
	FVector transformedForce = UKismetMathLibrary::TransformDirection(RelativeTransform, Force);
	Body->AddForceAtLocation(transformedForce, HitStruct.Location);

	/*FVector LinearForce;
	FVector AngularForce;*/

}

void ATrackedVehicle::CheckWheelCollision(int32 SuspIndex, TArray<FSuspensionInternalProcessing> SuspensionArray, ESide Side)
{
	switch (Side)
	{
	case ESide::Left:
		WheelAngularVelocity = TrackLeftAngularVelocity;
		WheelLinearVelocity = TrackLeftLinearVelocity;
		break;
	case ESide::Right:
		WheelAngularVelocity = TrackRightAngularVelocity;
		WheelLinearVelocity = TrackRightLinearVelocity;
		break;
	default:
		break;
	}

	index = SuspIndex;

	SuspensionLength = SuspensionArray[index].Length;
	SuspensionStiffness = SuspensionArray[index].Stiffness;
	SuspensionDamping = SuspensionArray[index].Damping;
	SuspensionPreviousLength = SuspensionArray[index].PreviousLength;
	
	SuspensionWorldX = UKismetMathLibrary::TransformDirection(GetActorTransform(), UKismetMathLibrary::GetForwardVector(SuspensionArray[index].RootRot));
	SuspensionWorldY = UKismetMathLibrary::TransformDirection(GetActorTransform(), UKismetMathLibrary::GetRightVector(SuspensionArray[index].RootRot));
	SuspensionWorldZ = UKismetMathLibrary::TransformDirection(GetActorTransform(), UKismetMathLibrary::GetUpVector(SuspensionArray[index].RootRot));

	SuspensionWorldLocation = UKismetMathLibrary::TransformDirection(GetActorTransform(), SuspensionArray[index].RootLoc);

	FVector Location;
	FVector ImpactPoint;
	FVector ImpactNormal;
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;
	UPrimitiveComponent* Component = nullptr;
	bool bHit = TraceForSuspension(
		SuspensionWorldLocation,
		SuspensionWorldLocation - SuspensionWorldZ * SuspensionLength,
		SuspensionArray[index].Radius,
		Location,
		ImpactPoint,
		ImpactNormal,
		SurfaceType,
		Component
	);
	
	if (bHit)
	{
		CollisionPrimitive = Component;
		PhysicMaterial = SurfaceType;
		SuspensionEngaged = true;
		SuspensionNewLength = (SuspensionWorldLocation - Location).Size();
		WheelCollisionLocation = Location;
		WheelCollisionNormal = ImpactNormal;

		AddSuspensionForce();

		SuspensionArray[index].PreviousLength = SuspensionNewLength;
		SuspensionArray[index].SuspensionForce = SuspensionForce;
		SuspensionArray[index].WheelCollisionLocation = WheelCollisionLocation;
		SuspensionArray[index].WheelCollisionNormal = WheelCollisionNormal;
		SuspensionArray[index].Engaged = SuspensionEngaged;
		SuspensionArray[index].HitMaterial = PhysicMaterial;

		PushSuspesionToEnvironment();
	}
	else
	{
		// If there is no collision then suspension is relaxed
		SuspensionForce = FVector();
		SuspensionNewLength = SuspensionLength;
		WheelCollisionLocation = FVector();
		WheelCollisionNormal = FVector();
		SuspensionEngaged = false;
	}
}

void ATrackedVehicle::AddSuspensionForce()
{
	float SpringSuspensionRatio = (SuspensionLength - SuspensionNewLength) / SuspensionLength;
	float SuspensionCelocity = (SuspensionNewLength - SuspensionPreviousLength) / GetWorld()->DeltaTimeSeconds;
	SuspensionForce = (UKismetMathLibrary::Clamp(SpringSuspensionRatio, 0, 1) * SuspensionStiffness + SuspensionDamping * (SuspensionTargetVelocity - SuspensionCelocity)) * SuspensionWorldZ;
	Body->AddForceAtLocation(SuspensionForce, SuspensionWorldLocation);
}

void ATrackedVehicle::PushSuspesionToEnvironment()
{
	if (ensure(CollisionPrimitive) && CollisionPrimitive->IsSimulatingPhysics())
	{
		CollisionPrimitive->AddForceAtLocation(-SuspensionForce, WheelCollisionLocation);
	}
}

FVector ATrackedVehicle::GetVelocityAtPoint(FVector PointLoc)
{
	FTransform bodyTransform = UKismetMathLibrary::MakeTransform(Body->GetCenterOfMass(), GetActorRotation(), GetActorScale3D());
	FVector localVelocityInDegree = UKismetMathLibrary::InverseTransformDirection(bodyTransform, Body->GetPhysicsAngularVelocityInDegrees());
	FVector localVelocityInRadius = FVector(
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.X),
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.Y),
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.Z)
	);
	FVector localLinearVelocity = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), Body->GetPhysicsLinearVelocity());
	FVector localAngualrVelocity = UKismetMathLibrary::Cross_VectorVector(localVelocityInRadius, UKismetMathLibrary::InverseTransformDirection(bodyTransform, PointLoc));
	FVector localVelocity = localLinearVelocity + localAngualrVelocity;

	
	return UKismetMathLibrary::TransformDirection(GetActorTransform(), localVelocity);

}

bool ATrackedVehicle::PutToSleep()
{
	if (SleepMode)
	{
		if (SleepDelayTimer < SleepTimerSeconds)
		{
			SleepDelayTimer += GetWorld()->DeltaTimeSeconds;
			return SleepMode;
		}
	}

	if (Body->GetPhysicsLinearVelocity().Size() < SleepVelocity
		&& SleepVelocity < Body->GetPhysicsAngularVelocityInDegrees().Size())
	{
		if (!SleepMode)
		{
			SleepMode = true;
			TArray<UActorComponent*> Components = GetComponentsByClass(UStaticMeshComponent::StaticClass());
			for (size_t i = 0; i < Components.Num(); i++)
			{
				Cast<UPrimitiveComponent>(Components[i])->PutRigidBodyToSleep();
				SleepDelayTimer = 0;
			}
		}
	}
	else
	{
		if (SleepMode)
		{
			SleepMode = false;
			SleepDelayTimer = 0;
		}
	}

	return SleepMode;
}

FVector ATrackedVehicle::GetVelocityAtPointWorld(FVector PointLoc)
{
	FVector localLinearVelocity = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), Body->GetPhysicsLinearVelocity());
	FVector localVelocityInDegree = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), Body->GetPhysicsAngularVelocityInDegrees());
	FVector localVelocityInRadius = FVector(
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.X),
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.Y),
		UKismetMathLibrary::DegreesToRadians(localVelocityInDegree.Z)
	);

	FVector localCenterOfMass = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), Body->GetCenterOfMass());
	FVector localAngualrVelocity = UKismetMathLibrary::Cross_VectorVector(localVelocityInRadius, UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), PointLoc) - localCenterOfMass);
	FVector localVelocity = localLinearVelocity + localAngualrVelocity;
	return UKismetMathLibrary::TransformDirection(GetActorTransform(), localVelocity);
}

void ATrackedVehicle::AddGravity()
{
	Body->AddForce(Body->GetMass() * FVector(0, 0, -980));
}

void ATrackedVehicle::UpdateThrottle()
{
	TrackTorqueTransferRight = UKismetMathLibrary::Clamp(WheelRightCoefficient + WheelForwardCoefficient, -1, 2);
	TrackTorqueTransferLeft = UKismetMathLibrary::Clamp(WheelLeftCoefficient + WheelForwardCoefficient, -1, 2);
	
	if (UKismetMathLibrary::Max(UKismetMathLibrary::Abs(TrackTorqueTransferRight), UKismetMathLibrary::Abs(TrackTorqueTransferLeft)) != 0)
	{
		ThrottleIncrement = 0.5;
	}
	else
	{
		ThrottleIncrement = -1;
	}

	UKismetMathLibrary::Clamp(GetWorld()->DeltaTimeSeconds * ThrottleIncrement + Throttle, 0, 1);
}

void ATrackedVehicle::UpdateWheelsVelocity()
{
	// 计算力矩
	TrackRightTorque = DriveRightTorque + TrackFrictionTorqueRight + TrackRollingFrictionTorqueRight;
	TrackLeftTorque = DriveLeftTorque + TrackFrictionTorqueLeft + TrackRollingFrictionTorqueLeft;

	// 计算角速度
	TrackRightAngularVelocity = ApplyBrake(TrackRightTorque / MomentInertia * GetWorld()->DeltaTimeSeconds + TrackRightAngularVelocity, BrakeRatioRight);
	TrackLeftAngularVelocity = ApplyBrake(TrackLeftTorque / MomentInertia * GetWorld()->DeltaTimeSeconds + TrackLeftAngularVelocity, BrakeRatioLeft);
	
	// 计算线速度
	TrackRightLinearVelocity = TrackRightAngularVelocity * SprocketRadiusCm;
	TrackLeftLinearVelocity = TrackLeftAngularVelocity * SprocketRadiusCm;

}

void ATrackedVehicle::UpdateAxlsVelocity()
{
	AxisAngularVelocity = (UKismetMathLibrary::Abs(TrackRightAngularVelocity) + UKismetMathLibrary::Abs(TrackLeftAngularVelocity)) / 2;
}

void ATrackedVehicle::CalculateEngineAndUpdateDrive()
{
	EngineTorque = GetEngineTorque(GetEngineRPMFromAxls(AxisAngularVelocity)) * Throttle;
	DriveAxlsTorque = GetGearBoxTorque(EngineTorque);

	DriveRightTorque = TrackTorqueTransferRight * DriveAxlsTorque;
	DriveLeftTorque = TrackTorqueTransferLeft * DriveAxlsTorque;

	DriveRightForce = GetActorForwardVector() * DriveRightTorque / SprocketRadiusCm;
	DriveLeftForce = GetActorForwardVector() * DriveLeftTorque / SprocketRadiusCm;
}

void ATrackedVehicle::CountFrictionContactPoint(TArray<FSuspensionInternalProcessing> SuspSide)
{
	for (size_t i = 0; i < SuspSide.Num(); i++)
	{
		if (SuspSide[i].Engaged)
		{
			TotalNumFrictionPoints++;
		}
	}
}

void ATrackedVehicle::ApplyDriveForceAndGetFrictionForceOnSide(TArray<FSuspensionInternalProcessing> SuspensionSide, FVector DriveForceSide, float TrackLinearVelocitySide, OUT float TotalFrictionTorqueSide, OUT float TotalRollingFrictionToqueSide)
{
	for (size_t i = 0; i < SuspensionSide.Num(); i++)
	{
		if (SuspensionSide[i].Engaged)
		{
			WheelLoadN = UKismetMathLibrary::ProjectVectorOnToVector(SuspensionSide[i].SuspensionForce, SuspensionSide[i].WheelCollisionNormal).Size();
			FVector combinedVelocity = GetVelocityAtPointWorld(SuspensionSide[i].WheelCollisionLocation) - UKismetMathLibrary::GetForwardVector(GetActorRotation()) * TrackLinearVelocitySide;
			RelativeTrackVelocity = UKismetMathLibrary::ProjectVectorOnToPlane(combinedVelocity, SuspensionSide[i].WheelCollisionNormal);

			GetMuFromFrictionElipse(
				RelativeTrackVelocity.GetSafeNormal(),
				UKismetMathLibrary::GetForwardVector(GetActorRotation()),
				MuXStatic,
				MuYStatic,
				MuXKinetic,
				MuYKinetic,
				MuStatic,
				MuKinetic
			);

			FVector FrictionForce = -Body->GetMass() * RelativeTrackVelocity / GetWorld()->DeltaTimeSeconds / TotalNumFrictionPoints;

			// Full friction force from vehicle movement
			FullStaticFrictionForce = UKismetMathLibrary::ProjectVectorOnToVector(
				FrictionForce,
				UKismetMathLibrary::ProjectVectorOnToPlane(UKismetMathLibrary::GetForwardVector(GetActorRotation()), SuspensionSide[i].WheelCollisionNormal).GetSafeNormal()
			) * MuXStatic + UKismetMathLibrary::ProjectVectorOnToVector(
				FrictionForce,
				UKismetMathLibrary::ProjectVectorOnToPlane(UKismetMathLibrary::GetRightVector(GetActorRotation()), SuspensionSide[i].WheelCollisionNormal).GetSafeNormal()
			) * MuYStatic;

			// Full friction force from vehicle movement
			FullKineticFrictionForce = UKismetMathLibrary::ProjectVectorOnToVector(
				FrictionForce,
				UKismetMathLibrary::ProjectVectorOnToPlane(UKismetMathLibrary::GetForwardVector(GetActorRotation()), SuspensionSide[i].WheelCollisionNormal).GetSafeNormal()
			) * MuXKinetic + UKismetMathLibrary::ProjectVectorOnToVector(
				FrictionForce,
				UKismetMathLibrary::ProjectVectorOnToPlane(UKismetMathLibrary::GetRightVector(GetActorRotation()), SuspensionSide[i].WheelCollisionNormal).GetSafeNormal()
			) * MuYKinetic;

			FullStaticDriveForce = MuXStatic * UKismetMathLibrary::ProjectVectorOnToPlane(DriveForceSide, SuspensionSide[i].WheelCollisionNormal);
			FullKineticDriveForce = MuXKinetic * UKismetMathLibrary::ProjectVectorOnToPlane(DriveForceSide, SuspensionSide[i].WheelCollisionNormal);


			// We want to apply higher friction if forces are bellow static friction limit
			if ((FullStaticFrictionForce + FullStaticDriveForce).Size() >= WheelLoadN * MuStatic)
			{
				FullFrictionForceNorm = FullKineticFrictionForce.GetSafeNormal();
				ApplicationForce = UKismetMathLibrary::ClampVectorSize(FullKineticFrictionForce + FullKineticDriveForce, 0, WheelLoadN * MuKinetic);
			}
			else
			{
				FullFrictionForceNorm = FullStaticFrictionForce.GetSafeNormal();
				ApplicationForce = UKismetMathLibrary::ClampVectorSize(FullStaticFrictionForce + FullStaticDriveForce, 0, WheelLoadN * MuKinetic);
			}
			Body->AddForceAtLocation(ApplicationForce, SuspensionSide[i].WheelCollisionLocation);
			FVector FrictionEffectTransmission = -UKismetMathLibrary::ProjectVectorOnToVector(ApplicationForce, FullFrictionForceNorm) / Body->GetMass() * (TrackMassKg + SprocketMassKg);
			TrackFrictionTorque = UKismetMathLibrary::ProjectVectorOnToVector(
				UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), FrictionEffectTransmission),
				FVector(1, 0, 0)
			).X * SprocketRadiusCm;

			// Make this force instead of torque
			TrackRollingFrictionTorque = -UKismetMathLibrary::SignOfFloat(TrackLinearVelocitySide) * WheelLoadN *
				(RollingFrictionCoeffient + UKismetMathLibrary::Abs(TrackLinearVelocitySide) * 0.000015);

			TotalTrackFrictionTorque += TrackFrictionTorque;
			TotalTrackRollingFrictionTorque += TrackRollingFrictionTorque;
		}
	}
	TotalFrictionTorqueSide = TotalTrackFrictionTorque;
	TotalRollingFrictionToqueSide = TotalTrackRollingFrictionTorque;
		
}

float ATrackedVehicle::GetEngineRPMFromAxls(float AxlsAngularVelocity)
{
	return (AxlsAngularVelocity * GearRatios[currentGear] * DifferentialRatio * 60) / UKismetMathLibrary::GetPI() / 2;
}

float ATrackedVehicle::GetGearBoxTorque(float EngineTorque)
{
	float GearBoxTorque = GearRatios[currentGear] * DifferentialRatio * TransmissionEfficiency * EngineTorque * EngineExtraPowerRatio;
	return ReverseGear ? -GearBoxTorque : GearBoxTorque;
}

float ATrackedVehicle::ApplyBrake(float AngularVelocity, float BrakeRatio)
{
	float BrakeImpulse = GetWorld()->DeltaTimeSeconds * BrakeForce * BrakeRatio;

	if (UKismetMathLibrary::Abs(AngularVelocity) > UKismetMathLibrary::Abs(BrakeImpulse))
	{
		return AngularVelocity - UKismetMathLibrary::SignOfFloat(AngularVelocity) * BrakeImpulse;
	}
	else
	{
		return 0.0;
	}
}

bool ATrackedVehicle::TraceForSuspension(FVector Start, FVector End, float Radius, OUT FVector Location, OUT FVector ImpactPoint, OUT FVector ImpactNormal, OUT EPhysicalSurface SufaceType, OUT UPrimitiveComponent * Component)
{
	FHitResult HitResult;
	GetWorld()->SweepSingleByChannel(
		HitResult, 
		Start, 
		End, 
		FQuat::Identity, 
		ECollisionChannel::ECC_Visibility, 
		FCollisionShape::MakeSphere(Radius)
	);

	if (HitResult.PhysMaterial.IsValid())
	{
		SufaceType = UGameplayStatics::GetSurfaceType(HitResult);
	}
	Location = HitResult.Location;
	ImpactPoint = HitResult.ImpactPoint;
	ImpactNormal = HitResult.ImpactNormal;
	Component = HitResult.GetComponent();

	return HitResult.bBlockingHit;
}

void ATrackedVehicle::ShiftGear(int32 ShiftUpOrDown)
{
	int32 min;
	int32 max;
	
	if (ReverseGear)
	{
		min = 0;
		max = NeutralGearIndex + 1;
	}
	else
	{
		min = NeutralGearIndex - 1;
		max = GearRatios.Num();
	}
	

	if (AutoGearBox)
	{
		currentGear = UKismetMathLibrary::Clamp(NeutralGearIndex + 1, min, max);
	}
	else
	{
		currentGear = UKismetMathLibrary::Clamp(currentGear + ShiftUpOrDown, 0, GearRatios.Num());
		ReverseGear = currentGear >= NeutralGearIndex ? false : true;
	}
}

void ATrackedVehicle::GetThrottleInputForAutoHandling(float InputVehicleLeftRight, float InputVehicleForwardBackward)
{
	AxisInputValue = InputVehicleForwardBackward;
	FVector localVelocity = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), GetVelocity());
	if (AxisInputValue != 0)
	{
		if (localVelocity.Size() > 10) // Are we moving?
		{
			if (AxisInputValue > 0) // Is forward pressed?
			{
				if (localVelocity.X > 0) // We are moving forward with forward pressed
				{
					// Forward
					Forward();
				}
				else
				{
					// Brake
					Brake();
				}
			}
			else 
			{
				if (localVelocity.X > 0) // We are moving forward with backwards pressed
				{
					// Brake
					Brake();
				}
				else
				{
					// Backwards
					Backward();
				}
			}
		}
		else
		{
			if (AxisInputValue > 0) // Not moving yet but forward/backward is pressed
			{
				// Forward
				Forward();
			}
			else
			{
				Backward();
			}
		}
	}
	else
	{
		// Forward/backward is not pressed, no throttle but maybe we are steering
		if (InputVehicleLeftRight != 0 && localVelocity.Size() > 10)
		{
			// We are steering without throttle and not rolling
			Forward();
		}
		else
		{
			UpdateCoefficient();
		}
	}
}

// Calculate Mu from friction elipse defined by MuX and MuY as radius of elipse
void ATrackedVehicle::GetMuFromFrictionElipse(FVector VelocityDirectionNormalized, FVector ForwardVector, float Mu_X_Static, float Mu_Y_Static, float Mu_X_Kinetic, float Mu_Y_Kinetic, OUT float Mu_Static, OUT float Mu_Kinetic)
{
	float forwardVelocity = UKismetMathLibrary::Dot_VectorVector(VelocityDirectionNormalized, ForwardVector);
	Mu_Static = FVector2D(Mu_X_Static * forwardVelocity, UKismetMathLibrary::Sqrt(1 - forwardVelocity * forwardVelocity) * Mu_Y_Static).Size();
	Mu_Kinetic = FVector2D(Mu_X_Kinetic * forwardVelocity, UKismetMathLibrary::Sqrt(1 - forwardVelocity * forwardVelocity) * Mu_Y_Kinetic).Size();
}

void ATrackedVehicle::Forward()
{
	ReverseGear = false;
	ShiftGear(0);
	UpdateCoefficient();
}

void ATrackedVehicle::Brake()
{
	BrakeRatioRight = UKismetMathLibrary::Abs(AxisInputValue);
	BrakeRatioLeft = UKismetMathLibrary::Abs(AxisInputValue);
	WheelForwardCoefficient = 0;
}

void ATrackedVehicle::Backward()
{
	ReverseGear = true;
	ShiftGear(0);
	UpdateCoefficient();
}

void ATrackedVehicle::UpdateCoefficient()
{
	BrakeRatioLeft = 0;
	BrakeRatioRight = 0;
	WheelForwardCoefficient = UKismetMathLibrary::Abs(AxisInputValue);
}




