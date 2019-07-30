// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrackedVehicle.h"
#include "Tank_FR.generated.h"

/**
 *
 */
UCLASS()
class TANK_API ATank_FR : public ATrackedVehicle
{
	GENERATED_BODY()
public:
	ATank_FR();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 主动轮： 动力输出部件，驱动履带进行转动，带动坦克行驶，转向和刹车等功能也通过主动轮进行差速和制动操作来实现
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* RightSprocket;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftSprocket;

	// 负重轮：没用动力的从动轮，用于将悬挂承载的车辆重量传导到履带上面，同时规整履带，强迫履带按直线运动
	TArray<UStaticMeshComponent*> LeftRoadWheels;
	TArray<UStaticMeshComponent*> RightRoadWheels;

	// 惰轮 用来诱导履带围绕轮组完成转动，并用来控制履带张紧度的从动轮
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* RightIdler;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftIdler;

	// 履带
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* RightTreads;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* LeftTreads;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* RightTrackSpline;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* LeftTrackSpline;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Burrel;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Suspensions;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* combParts;

	virtual void AnimateWheels() override;
	virtual void AnimateTreadsSpline() override;
	virtual void RegisterSuspensionHandles() override;
};
