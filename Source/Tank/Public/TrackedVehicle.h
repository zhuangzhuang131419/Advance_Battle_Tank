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
#include "Components/SplineComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/Transform.h"
#include "GeneralTypes.h"
#include "TrackedVehicle.generated.h"

UCLASS()
class TANK_API ATrackedVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrackedVehicle();

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Body;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UArrowComponent* COM;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USkeletalMeshComponent* TreadR;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USkeletalMeshComponent* TreadL;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* WheelSweep;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Turret;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USpringArmComponent* MainCam;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USkeletalMeshComponent* Cannon;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USpringArmComponent* TurretCam;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USpringArmComponent* Front;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USpringArmComponent* LookRight;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USpringArmComponent* LookLeft;
	// 履带
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* RightTreads;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* LeftTreads;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* RightTrackSpline;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* LeftTrackSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool SleepMode = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ProjectileClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Construct
	UFUNCTION(BlueprintCallable)
	void BuildTrackSpline(USplineComponent* RightSpline, USplineComponent* LeftSpline, UInstancedStaticMeshComponent* TreadsRight, UInstancedStaticMeshComponent* TreadsLefts);
	UFUNCTION(BlueprintCallable)
	void PreCalculateMomentOfInteria();
	UFUNCTION(BlueprintCallable)
	void ConstructSuspension();
	UFUNCTION(BlueprintCallable)
	void VisualizeCenterOfMass();
	void CreateMaterialsForSimpleTracks();
	UFUNCTION(BlueprintCallable)
	virtual void RegisterSuspensionHandles();
	UFUNCTION(BlueprintCallable)
	void FindNeutralGearAndSetStartingGear();

	void AddWheelForce(UPrimitiveComponent* Wheel, FVector Force);
	void AddWheelForceImproved(UPrimitiveComponent* Wheel, FVector Force, FHitResult HitStruct, UPhysicsConstraintComponent* Suspension);
	void CheckWheelCollision(int32 SuspIndex, TArray<FSuspensionInternalProcessing> SuspensionArray, ESide Side);
	FVector GetVelocityAtPoint(FVector PointLoc);
	bool PutToSleep();
	FVector GetVelocityAtPointWorld(FVector PointLoc);
	void GetTotalSuspensionForce();
	void AddGravity();
	void PositionAndAnimateDriveWheels(UStaticMeshComponent* WheelComponent, FSuspensionInternalProcessing SuspensionSet, int32 SuspensionIndex, ESide side, bool FlipAnimation180Degrees);
	void UpdateThrottle();
	void UpdateWheelsVelocity();
	float GetWheelAccelerationFromEngineTorque(float Torque);
	void ApplyDrag();
	float GetEngineTorque(float RevolutionPerMinute);
	virtual void AnimateWheels();
	void UpdateAxlsVelocity();

	void CalculateEngineAndUpdateDrive();
	void CountFrictionContactPoint(TArray<FSuspensionInternalProcessing> SuspSide);
	void ApplyDriveForceAndGetFrictionForceOnSide(TArray<FSuspensionInternalProcessing> SuspensionSide, FVector DriveForceSide, float TrackLinearVelocitySide, OUT float TotalFrictionTorqueSide, OUT float TotalRollingFrictionToqueSide);
	float GetGearBoxTorque(float EngineTorque);
	float GetEngineRPMFromAxls(float AxlsAngularVelocity);
	// 刹车
	float ApplyBrake(float AngularVelocity, float BrakeRatio);
	virtual void AnimateTreadsSpline();
	void AnimateSprocketOrIdler(UStaticMeshComponent* SprocketOrIdlerComponnet, float TrackAngularVelocity, bool FlipAnimation180Degrees);
	void ShowSuspensionHandles();
	void SpawnDust(TArray<FSuspensionInternalProcessing>& SuspensionSide, float TrackLinearVelocity);

	bool TraceForSuspension(FVector Start, FVector End, float Radius, OUT FVector Location, OUT FVector ImpactPoint, OUT FVector ImpactNormal, OUT EPhysicalSurface SufaceType, OUT UPrimitiveComponent* Component);
	void AnimateTreadsMaterial();
	void AnimateTreadsInstancedMesh(USplineComponent* RightSpline, USplineComponent* LeftSpline, UInstancedStaticMeshComponent* TreadsRight, UInstancedStaticMeshComponent* ThreadsLeft);
	void AnimateTreadsSplineControlPoints(UStaticMeshComponent* WheelMeshComponent, USplineComponent* ThreadSplineComponent, int32 BottomCPIndex, int32 TopCPIndex, TArray<FVector> SplineCoordinates, TArray<FSuspensionSetUp> SuspensionSetUp, int32 SuspensionIndex);
	void ShiftGear(int32 ShiftUpOrDown);
	void UpdateAutoGearBox();

	UFUNCTION(BlueprintImplementableEvent)
	void SetRemoveAutoGearBoxTimer(bool bSetTimer);

	UFUNCTION(BlueprintCallable)
	void GetThrottleInputForAutoHandling(float InputVehicleLeftRight, float InputVehicleForwardBackward);
	void GetGearBoxInfo(OUT int32 GearNum, OUT bool ReverseGear, OUT bool Automatic);
	void GetMuFromFrictionElipse(FVector VelocityDirectionNormalized, FVector ForwardVector, float Mu_X_Static, float Mu_Y_Static, float Mu_X_Kinetic, float Mu_Y_Kinetic, OUT float Mu_Static, OUT float Mu_Kinetic);

	//Input
	// Action
	void Fire();

private:
	void AddSuspensionForce();
	void PushSuspesionToEnvironment();
	void Forward();
	void Brake();
	void Backward();
	void UpdateCoefficient();

public:
	// 履带配置
	UPROPERTY(EditDefaultsOnly, Category = "Track")
	float TrackMassKg = 600.f;

	// 主动轮配置
	UPROPERTY(EditDefaultsOnly, Category = "Sprocket")
	float SprocketMassKg = 65.f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprocket")
	float SprocketRadiusCm = 25.f;

	// 环境配置
	UPROPERTY(EditDefaultsOnly, Category = "Environment")
	float AirDensity = 1.29f;
	UPROPERTY(EditDefaultsOnly, Category = "Environment")
	UMaterialInterface* Dust;
	UPROPERTY(EditDefaultsOnly, Category = "Environment")
	UParticleSystem* DustSmoke;


	// Drag相关
	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	float DragSurfaceArea = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	float DragCoefficient = 0.8f;

	// Gear配置
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	TArray<float> GearRatios = { 1.15, 2.15, 4.35, 0, 3.81, 1.93, 1 };
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	float DifferentialRatio = 3.5; // 差速比
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	float TransmissionEfficiency = 0.9; // 能量传递效率
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	bool AutoGearBox = true; // 坦克无法在false的时候开动
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	float GearUpShiftPrc = 0.9;
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	float GearDownShiftPrc = 0.9;
	UPROPERTY(EditDefaultsOnly, Category = "Gear")
	float EngineExtraPowerRatio;

	// Torque
	UPROPERTY(EditDefaultsOnly, Category = "Torque")
	UCurveFloat* EngineTorqueCurve;

	// 摩擦力
	UPROPERTY(EditDefaultsOnly, Category = "Friction")
	float MuXStatic = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Friction")
	float MuYStatic = 0.85;
	UPROPERTY(EditDefaultsOnly, Category = "Friction")
	float MuXKinetic = 0.5;
	UPROPERTY(EditDefaultsOnly, Category = "Friction")
	float MuYKinetic = 0.45;
	UPROPERTY(EditDefaultsOnly, Category = "Friction")
	float RollingFrictionCoeffient = 0.02;

	// Spline
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Spline")
	TArray<FVector> SplineCoordinatesRight;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Spline")
	TArray<FVector> SplineCoordinatesLeft;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Spline")
	TArray<FVector> SplineTangents;

	// Brake
	UPROPERTY(EditDefaultsOnly, Category = "Brake")
	float BrakeForce = 30.f;

	// Tread
	UPROPERTY(EditDefaultsOnly, Category = "Tread")
	float TreadUVTiles = 32.5f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Tread")
	float TreadsOnSide = 64;
	UPROPERTY(EditDefaultsOnly, Category = "Tread")
	float TreadHalfThickness = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Tread")
	float TreadLength = 972.5f;


	// Spline
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	TArray<FSuspensionSetUp> SuspensionSetUpRight;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	TArray<FSuspensionSetUp> SuspensionSetUpLeft;

	// Sleep
	UPROPERTY(EditDefaultsOnly, Category = "Sleep")
	float SleepVelocity = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Sleep")
	float SleepTimerSeconds = 2;

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



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelRightCoefficient = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelLeftCoefficient = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelForwardCoefficient = 50;

	float WheelLoadN;

	float Throttle;
	float ThrottleIncrement;

	float AxisAngularVelocity;

	// Friction相关
	float TotalNumFrictionPoints;

	float MuStatic;
	float MuKinetic;

	FVector FullStaticFrictionForce;
	FVector FullKineticFrictionForce;

	FVector FullStaticDriveForce;
	FVector FullKineticDriveForce;

	FVector FullFrictionForceNorm;


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
	float EngineRPM;

	// 受力有关
	FVector DriveRightForce;
	FVector DriveLeftForce;
	FVector ApplicationForce;

	// 转动惯量
	float MomentInertia;

	// 刹车有关
	UPROPERTY(BlueprintReadWrite, Category = "Brake")
	float BrakeRatioRight;
	UPROPERTY(BlueprintReadWrite, Category = "Brake")
	float BrakeRatioLeft;

	// Suspension
	TArray<UStaticMeshComponent*> SuspensionHandleRight;
	TArray<UStaticMeshComponent*> SuspensionHandleLeft;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalRight;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalLeft;


	FVector SuspensionForce;
	float SuspensionTargetVelocity;
	FVector TotalSuspensionForce;

	// Gear齿轮相关
	int32 currentGear = 1;
	int32 NeutralGearIndex = 0;
	bool ReverseGear;
	float LastAutoGearBoxAxisCheck;

	// Tread相关

	float TreadUVOffsetRight;
	float TreadUVOffsetLeft;
	float TreadMeshOffsetRight;
	float TreadMeshOffsetLeft;
	UMaterialInstanceDynamic* TreadMaterialRight;
	UMaterialInstanceDynamic* TreadMaterialLeft;
	UInstancedStaticMeshComponent* TreadsRightLoc;
	UInstancedStaticMeshComponent* TreadsLeftLoc;
	int32 TreadsLastIndex = 63;

	// Spline相关
	USplineComponent* SplineRightLoc;
	USplineComponent* SplineLeftLoc;
	float SplineLengthAtConstruction;


	// Time
	UPROPERTY(BlueprintReadWrite, Category = "Timer")
	float SleepDelayTimer;
	UPROPERTY(BlueprintReadWrite, Category = "Timer")
	FTimerHandle AutoGearBoxTimerHandle;

private:
	// Axis相关
	float AxisInputValue;

	// 车轮的速度
	float WheelAngularVelocity;
	float WheelLinearVelocity;

	// Suspension
	float SuspensionLength;
	float SuspensionNewLength;
	float SuspensionStiffness;
	float SuspensionDamping;
	float SuspensionPreviousLength;
	FVector SuspensionWorldX;
	FVector SuspensionWorldY;
	FVector SuspensionWorldZ;
	FVector SuspensionWorldLocation;
};
