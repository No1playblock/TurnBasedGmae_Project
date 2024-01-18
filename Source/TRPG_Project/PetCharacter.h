// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleCharacter.h"

#include "TurnManager.h"
#include "PetCharacter.generated.h"

/**
 *
 */
UCLASS()
class TRPG_PROJECT_API APetCharacter : public ABattleCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APetCharacter();
	virtual void doTurn(TArray<ABattleCharacter*> CollidingActors, int index) override;
	void turnEnd(TArray<ABattleCharacter*> CollidingActors, int index);
	virtual void TakeDamage(int atk) override;

	//void Attack(APlayableCharacter* player);

	void Attack(TArray<ABattleCharacter*> CollidingActors, int index);
	void Defense();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo, Meta = (AllowPrivateAccess = "true"))
	EBehaviorType behaviorType;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackedMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoxMesh;

	void IsPunchAnimationPlaying();

	void IsAttackedAnimationPlaying();

	void PlayPunchMontage(float PlayRate = 1.0f);
	void PlayAttackedMontage(float PlayRate = 1.0f);

	FTimerHandle AnimationHandle;

	class TArray<ABattleCharacter*> battleActors;

	int battleActors_index;

	class ATurnManager* TurnManager;

	float petHealthPoint;
	float petMaxHealthPoint;
	float petAttackPower;
	float petDefensePower;

	bool isChild = false;

	FString name;

	class APlayableCharacter* player;
	TArray<ABattleCharacter*> TargetActors;						//플레이어와 몹만 골라오기
	int pet = 0;						//플레이어와 몹만 골라오기

};
