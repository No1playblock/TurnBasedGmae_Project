// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

UENUM()
enum class EBehaviorType : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Attack UMETA(DisplayName = "Attack"),
	Defense UMETA(DisplayName = "Defense"),
	Capture UMETA(DisplayName = "Capture")
};

UCLASS()
class TRPG_PROJECT_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

	virtual void doTurn(TArray<ABattleCharacter*> CollidingActors, int index);
	virtual void TakeDamage(int atk);
	FString ActorTypeTag = "Default";

};
