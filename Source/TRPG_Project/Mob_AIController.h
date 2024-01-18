// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Mob_AIController.generated.h"

/**
 *
 */
UCLASS()
class TRPG_PROJECT_API AMob_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AMob_AIController();
	virtual void OnPossess(APawn* InPawn) override;
	void IsNotBattle();

	void IsBattle();
	virtual void OnUnPossess() override;

private:
	FVector StartLocation;
	//FVector RandomLocation;

	FTimerHandle RepeatTimerHandle;
	float RepeatInterval;

	void MovetoRandomPoint();
};
