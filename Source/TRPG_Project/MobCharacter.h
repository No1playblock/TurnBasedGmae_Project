// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayableCharacter.h"
#include "MobCharacter.generated.h"


UCLASS()
class TRPG_PROJECT_API AMobCharacter : public ABattleCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMobCharacter();
	virtual void doTurn(TArray<ABattleCharacter*> CollidingActors, int index) override;
	void turnEnd(TArray<ABattleCharacter*> CollidingActors, int index);
	virtual void TakeDamage(int atk) override;

	//void Attack(APlayableCharacter* player);

	void Attack(TArray<ABattleCharacter*> CollidingActors, int index);
	void Defense();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo, Meta = (AllowPrivateAccess = "true"))
	EBehaviorType behaviorType;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackedMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void IsPunchAnimationPlaying();

	void IsAttackedAnimationPlaying();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoxMesh;

	void PlayPunchMontage(float PlayRate = 1.0f);
	void PlayAttackedMontage(float PlayRate = 1.0f);



	class ATurnManager* TurnManager;

	float mobHealthPoint;
	float mobMaxHealthPoint;
	float mobAttackPower;
	float mobDefensePower;

	class TArray<ABattleCharacter*> battleActors;

	int battleActors_index;

	FTimerHandle AnimationHandle;

	bool isGameOver = false;
	bool isChild = false;

	FString name;

	TArray<AMobCharacter*> ChildMobCharacter;				//haveMobCharacter 의 havMobCharacter는 0이어야 한다.
	TArray<ABattleCharacter*> TargetActors;						//플레이어와 몹만 골라오기

};
