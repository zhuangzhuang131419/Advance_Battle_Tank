// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackedVehicle.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"

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
	for each (UStaticMeshComponent* var in SuspensionHandleRight)
	{
		//SuspensionsInternalRight.Add(FSuspensionInternalProcessing(
		//))
		//var->GetRelativeTransform()
	}
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

void ATrackedVehicle::CheckWheelCollision(int32 SuspIndex, ESide Side)
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

float ATrackedVehicle::ApplyBrake(float AngularVelocity, float BrakeRatio)
{
	return 0.0;
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

float ATrackedVehicle::GetEngineRPMFromAxls(float AxlsAngularVelocity)
{
	return 0.0f;
}



