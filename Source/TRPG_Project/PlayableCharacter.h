// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.h"
#include "PlayableCharacter.generated.h"

class UAnimMontage;

UCLASS()
class TRPG_PROJECT_API APlayableCharacter : public ABattleCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayableCharacter();


	virtual void doTurn(TArray<ABattleCharacter*> CollidingActors, int index) override;

	virtual void TakeDamage(int atk) override;

	void turnEnd(TArray<ABattleCharacter*> CollidingActors, int index);
	void doPlayerTurn();
	//void doPlayerTurn(TArray<AActor*> CollidingActors, int index);

	void Attack();
	void Defense();
	void Capture();
	void BattleEnd();
	void GameOver();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo, Meta = (AllowPrivateAccess = "true"))
	EBehaviorType behaviorType;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MouseClicktoMove();
	void onMobClickedAttack();

	void IsPunchAnimationPlaying();

	void IsAttackedAnimationPlaying();



	void PlayPunchMontage(float PlayRate);

	void PlayAttackedMontage(float PlayRate);

	FRotator RotationCorrection(FRotator rot);


	void onMobClickedCapture();
	bool CalculateCapturePosibility(float mobHP, float mobMaxHP);
	void ToggleInventory();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void CreatePet();

	void CreateChildMob();

	/*UPROPERTY(EditAnywhere, Category = "playerInfo")
	float  moveSpeed = 1.0f;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverView;

	UPROPERTY()
	UUserWidget* WidgetInstance;

	float playerHealthPoint;

	float playerDefensePower;

	float playerAttackPower;


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))		//카메라 팔
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))		//카메라컴포넌트
		class UCameraComponent* FollowCamera;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABattleCharacter> MobToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABattleCharacter> PetToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackedMontage;

	int capturePosibility = 30;

	bool isInventoryOpen;

	FTimerHandle TurnTimerHandle;
	FTimerHandle AnimationHandle;

	class TArray<ABattleCharacter*> battleActors;

	int battleActors_index;

	class ATurnManager* TurnManager;

	class TArray<ABattleCharacter*> myPets;

	FVector2D screenSize;

	bool isInBattle = false;

};
