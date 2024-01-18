// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleCharacter.h"
#include "TurnManager.generated.h"

UCLASS()
class TRPG_PROJECT_API ATurnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void nextTurn(TArray<ABattleCharacter*> CollidingActors, int index);

	//void StartBattleTimer(TArray<AActor*> CollidingActors);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))		//카메라 팔
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))		//카메라컴포넌트
		class UCameraComponent* FollowCamera;
	//void BeginBattle();

	FTimerHandle BattleTimerHandle;		//배틀이 진행되는 타이머핸들

	TArray<ABattleCharacter*> BattleActors;		//배틀에 참여한 액터들
};
