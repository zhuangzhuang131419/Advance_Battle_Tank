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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PreCalculateMomentOfInteria();
	void ConstructSuspension();

	void AddWheelForce(UPrimitiveComponent* Wheel, FVector Force);

	void AddWheelForceImproved(UPrimitiveComponent* Wheel, FVector Force, FHitResult HitStruct, UPhysicsConstraintComponent* Suspension);

	void CheckWheelCollision(int32 SuspIndex, ESide Side);

	FVector GetVelocityAtPoint(FVector PointLoc);
	FVector GetVelocityAtPointWorld(FVector PointLoc);

	void AddGravity();

	void UpdateThrottle();

	void UpdateWheelsVelocity();

	// ɲ��
	float ApplyBrake(float AngularVelocity, float BrakeRatio);

	UFUNCTION(BlueprintImplementableEvent)
	float GetEngineTorque(float RevolutionPerMinute);

	void UpdateAxlsVelocity();

	void CalculateEngineAndUpdateDrive();
	float GetEngineRPMFromAxls(float AxlsAngularVelocity);
	float GetGearBoxTorque(float EngineTorque);

private:
	// UPROPERTY(BlueprintCallable)
	UPrimitiveComponent* WheelLoc;

	FTransform RelativeTransform;

	// �Ĵ����ٶ�
	float TrackRightAngularVelocity;
	float TrackLeftAngularVelocity;
	float TrackRightLinearVelocity;
	float TrackLeftLinearVelocity;

	// ���ֵ��ٶ�
	float WheelAngularVelocity;
	float WheelLinearVelocity;

	float WheelRightCoefficient;
	float WheelLeftCoefficient;
	float WheelForwardCoefficient;

	float Throttle;
	float ThrottleIncrement;

	float AxisAngularVelocity;

	// �������
	float TrackTorqueTransferRight;
	float TrackTorqueTransferLeft;

	float TrackRightTorque;
	float TrackLeftTorque;

	float DriveRightTorque;
	float DriveLeftTorque;
	float DriveAxlsTorque;

	float TrackFrictionTorqueRight;
	float TrackFrictionTorqueLeft;
	float TrackRollingFrictionTorqueRight;
	float TrackRollingFrictionTorqueLeft;

	float EngineTorque;

	// �����й�
	FVector DriveRightForce;
	FVector DriveLeftForce;

	// ת������
	float MomentInertia;

	// ɲ���й�
	float BrakeRatioRight;
	float BrakeRatioLeft;

	// �����й�
	float SprocketMassKg = 65.f;
	float SprocketRadiusCm = 25.f;

	// �Ĵ��й�
	float TrackMassKg = 600.f;

	// Suspension
	TArray<UStaticMeshComponent*> SuspensionHandleRight;
	TArray<UStaticMeshComponent*> SuspensionHandleLeft;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalRight;
	TArray<FSuspensionInternalProcessing> SuspensionsInternalLeft;
	TArray<FSuspensionSetUp> SuspensionSetUpRight;
	TArray<FSuspensionSetUp> SuspensionSetUpLeft;

	int32 index;

};
