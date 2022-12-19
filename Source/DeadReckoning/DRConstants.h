// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DRConstants.generated.h"

/**
 * 
 */
UCLASS()
class DEADRECKONING_API UDRConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static const TCHAR* AxisMoveForward = TEXT("MoveForward");
	inline static const TCHAR* AxisStrafeRight = TEXT("StrafeRight");
	inline static const TCHAR* AxisLookUp = TEXT("LookUp");
	inline static const TCHAR* AxisLookRight = TEXT("LookRight");

	inline static const TCHAR* ActionAttack = TEXT("Attack");
	inline static const TCHAR* ActionPlaceTorchScan = TEXT("PlaceTorchScan");
	inline static const TCHAR* ActionOpenMap = TEXT("OpenMap");

	static constexpr ECollisionChannel TraceChannelEnemy = ECC_GameTraceChannel2;
};
