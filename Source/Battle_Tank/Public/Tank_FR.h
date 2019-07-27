// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrackedVehicle.h"
#include "Tank_FR.generated.h"

/**
 * 
 */
UCLASS()
class BATTLE_TANK_API ATank_FR : public ATrackedVehicle
{
	GENERATED_BODY()
public:
	ATank_FR();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// �����֣� ������������������Ĵ�����ת��������̹����ʻ��ת���ɲ���ȹ���Ҳͨ�������ֽ��в��ٺ��ƶ�������ʵ��
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* RightSprocket;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftSprocket;

	// �����֣�û�ö����ĴӶ��֣����ڽ����ҳ��صĳ��������������Ĵ����棬ͬʱ�����Ĵ���ǿ���Ĵ���ֱ���˶�
	TArray<UStaticMeshComponent*> LeftRoadWheels;
	TArray<UStaticMeshComponent*> RightRoadWheels;

	// ���� �����յ��Ĵ�Χ���������ת���������������Ĵ��Ž��ȵĴӶ���
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* RightIdler;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftIdler;

	// �Ĵ�
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* RightThread;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* LeftThread;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	TArray<UStaticMeshComponent*> LeftSuspensions;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	TArray<UStaticMeshComponent*> RightSuspensions;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* RightTrackSpline;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	USplineComponent* LeftTrackSpline;
	
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* tail;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Suspensions;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* combParts;

	virtual void AnimateWheels() override;
	virtual void AnimateTreadsSpline() override;

private:
	int32 wheelsNum = 6;
	
};
