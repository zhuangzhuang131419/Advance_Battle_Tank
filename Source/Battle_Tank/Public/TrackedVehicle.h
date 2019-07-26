// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SplineComponent.h"
#include "Math/Transform.h"
#include "GeneralTypes.h"
#include "TrackedVehicle.generated.h"

UCLASS()
class BATTLE_TANK_API ATrackedVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrackedVehicle();

	UStaticMeshComponent* Body;
	UArrowComponent* COM;
	USkeletalMeshComponent* ThreadR;
	USkeletalMeshComponent* ThreadL;
	UStaticMeshComponent* WheelSweep;
	UStaticMeshComponent* Turrent;
	USpringArmComponent* MainCam;
	UCameraComponent* Camera;
	USkeletalMeshComponent* Cannon;
	USpringArmComponent* TurrentCam;
	USpringArmComponent* Front;
	USpringArmComponent* LookRight;
	USpringArmComponent* LookLeft;

	UPROPERTY(EditDefaultsOnly)
	bool SleepMode;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Construct
	void PreCalculateMomentOfInteria();
	void ConstructSuspension();
	void VisualizeCenterOfMass();
	UFUNCTION(BlueprintImplementableEvent)
	void CreateMaterialsForSimpleTracks();

	void AddWheelForce(UPrimitiveComponent* Wheel, FVector Force);
	void AddWheelForceImproved(UPrimitiveComponent* Wheel, FVector Force, FHitResult HitStruct, UPhysicsConstraintComponent* Suspension);
	void CheckWheelCollision(int32 SuspIndex, TArray<FSuspensionInternalProcessing> SuspensionArray, ESide Side);
	FVector GetVelocityAtPoint(FVector PointLoc);
	bool PutToSleep();
	FVector GetVelocityAtPointWorld(FVector PointLoc);
	void AddGravity();
	void UpdateThrottle();
	void UpdateWheelsVelocity();

	UFUNCTION(BlueprintImplementableEvent)
	float GetEngineTorque(float RevolutionPerMinute);

	void UpdateAxlsVelocity();

	void CalculateEngineAndUpdateDrive();
	void CountFrictionContactPoint(TArray<FSuspensionInternalProcessing> SuspSide);
	void ApplyDriveForceAndGetFrictionForceOnSide(TArray<FSuspensionInternalProcessing> SuspensionSide, FVector DriveForceSide, float TrackLinearVelocitySide, OUT float TotalFrictionTorqueSide, OUT float TotalRollingFrictionToqueSide);
	float GetEngineRPMFromAxls(float AxlsAngularVelocity);
	float GetGearBoxTorque(float EngineTorque);
	// 刹车
	float ApplyBrake(float AngularVelocity, float BrakeRatio);

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDust(TArray<FSuspensionInternalProcessing>& SuspensionSide, float TrackLinearVelocity);

	bool TraceForSuspension(FVector Start, FVector End, float Radius, OUT FVector Location, OUT FVector ImpactPoint, OUT FVector ImpactNormal, OUT EPhysicalSurface SufaceType, OUT UPrimitiveComponent* Component);

	void ShiftGear(int32 ShiftUpOrDown);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAutoGearBox();

	UFUNCTION(BlueprintImplementableEvent)
	void SetRemoveAutoGearBoxTimer();

	void GetThrottleInputForAutoHandling(float InputVehicleLeftRight, float InputVehicleForwardBackward);
	// void GetGearBoxInfo(OUT int32 GearNum, OUT bool ReverseGear, OUT bool Automatic);
	void GetMuFromFrictionElipse(FVector VelocityDirectionNormalized, FVector ForwardVector, float Mu_X_Static, float Mu_Y_Static, float Mu_X_Kinetic, float Mu_Y_Kinetic, OUT float Mu_Static, OUT float Mu_Kinetic);

private:
	// UPROPERTY(BlueprintCallable)
	UPrimitiveComponent* WheelLoc;


	
	EPhysicalSurface PhysicMaterial;

	// Collision相关
	UPrimitiveComponent* CollisionPrimitive;
	FVector WheelCollisionLocation;
	FVector WheelCollisionNormal;

	FTransform RelativeTransform;

	// 履带的速度
	float TrackRightAngularVelocity;
	float TrackLeftAngularVelocity;
	float TrackRightLinearVelocity;
	float TrackLeftLinearVelocity;

	FVector RelativeTrackVelocity;

	// 车轮的速度
	float WheelAngularVelocity;
	float WheelLinearVelocity;

	float WheelRightCoefficient;
	float WheelLeftCoefficient;
	float WheelForwardCoefficient;

	float WheelLoadN;

	float Throttle;
	float ThrottleIncrement;

	float AxisAngularVelocity;

	// Friction相关
	float TotalNumFrictionPoints;
	float MuXStatic = 1;
	float MuYStatic = 0.85;
	float MuXKinetic = 0.5;
	float MuYKinetic = 0.45;
	float MuStatic;
	float MuKinetic;

	FVector FullStaticFrictionForce;
	FVector FullKineticFrictionForce;

	FVector FullStaticDriveForce;
	FVector FullKineticDriveForce;

	FVector FullFrictionForceNorm;
	float RollingFrictionCoeffient = 0.02;

	// 力矩相关
	float TrackTorqueTransferRight;
	float TrackTorqueTransferLeft;
	float TrackRightTorque;
	float TrackLeftTorque;

	float DriveRightTorque;
	float DriveLeftTorque;
	float DriveAxlsTorque;

	float TrackFrictionTorque;
	float TrackFrictionTorqueRight;
	float TrackFrictionTorqueLeft;
	float TrackRollingFrictionTorque;
	float TrackRollingFrictionTorqueRight;
	float TrackRollingFrictionTorqueLeft;
	float TotalTrackFrictionTorque;
	float TotalTrackRollingFrictionTorque;

	float EngineTorque;

	// 受力有关
	FVector DriveRightForce;
	FVector DriveLeftForce;

	FVector ApplicationForce;


	// 转动惯量
	float MomentInertia;

	// 刹车有关
	float BrakeRatioRight;
	float BrakeRatioLeft;
	float BrakeForce = 30.f;

	// 齿轮有关
	float SprocketMassKg = 65.f;
	float SprocketRadiusCm = 25.f;

	// 履带有关
	float TrackMassKg = 600.f;

	// Suspension
	TArray<UStaticMeshComponent*> SuspensionHandleRight;
	TArray<UStaticMeshComponent*> SuspensionHandleLeft;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalRight;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalLeft;
	TArray<FSuspensionSetUp> SuspensionSetUpRight;
	TArray<FSuspensionSetUp> SuspensionSetUpLeft;
	float SuspensionLength;
	float SuspensionNewLength;
	float SuspensionStiffness;
	float SuspensionDamping;
	float SuspensionPreviousLength;
	FVector SuspensionWorldX;
	FVector SuspensionWorldY;
	FVector SuspensionWorldZ;
	FVector SuspensionWorldLocation;
	bool SuspensionEngaged;
	FVector SuspensionForce;
	float SuspensionTargetVelocity;

	// Gear齿轮相关
	int32 currentGear = 1;
	int32 NeutralGearIndex = 0;
	TArray<float> GearRatios = { 1.15, 2.15, 4.35, 0, 3.81, 1.93, 1 };

	float DifferentialRatio = 3.5; // 差速比
	float TransmissionEfficiency = 0.9; // 能量传递效率
	bool ReverseGear;
	bool AutoGearBox;

	float LastAutoGearBoxAxisCheck;
	float EngineExtraPowerRatio;

	// Axis相关
	float AxisInputValue;


	int32 index;



	// Sleep相关
	float SleepVelocity = 5;
	float SleepDelayTimer;
	float SleepTimerSeconds = 2;

	void AddSuspensionForce();
	void PushSuspesionToEnvironment();
	void Forward();
	void Brake();
	void Backward();
	void UpdateCoefficient();
};
