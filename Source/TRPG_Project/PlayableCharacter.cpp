// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "PetCharacter.h"
#include "Components/WidgetComponent.h"
#include "TurnManager.h"
#include "Mob_AIController.h"
#include "Pet_AIController.h"
#include "MobCharacter.h"

#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include <Kismet/KismetMathLibrary.h>
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
APlayableCharacter::APlayableCharacter() :
	isInventoryOpen(false),
	playerHealthPoint(100.0f),					//기본 체력
	playerAttackPower(20.0f),					//기본 공격력
	playerDefensePower(0.0f)					//기본 방어력
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorTypeTag = "Player";


	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 1500.0f;
	CameraArm->SetRelativeRotation(FRotator(-30.f, 180.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	behaviorType = EBehaviorType::Idle;
}
void APlayableCharacter::TakeDamage(int atk)
{
	if (playerDefensePower - atk < 0)
	{
		playerHealthPoint += (playerDefensePower - atk);

	}
	PlayAttackedMontage(1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Player HP: %f"), playerHealthPoint);

}



void APlayableCharacter::doTurn(TArray<ABattleCharacter*> CollidingActors, int index)        // doTurn이 상시 실행이 아니라서 스위치를 못함.
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerdoTurn"));

	if (GetWorldTimerManager().IsTimerActive(TurnTimerHandle)) {
		//UE_LOG(LogTemp, Warning, TEXT("return"));
		return;
	}
	battleActors = CollidingActors;
	battleActors_index = index;

	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &APlayableCharacter::doPlayerTurn, 0.1f, true);
}


void APlayableCharacter::turnEnd(TArray<ABattleCharacter*> CollidingActors, int index)
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	TurnManager->nextTurn(CollidingActors, index);
}

void APlayableCharacter::doPlayerTurn()
{
	switch (behaviorType)
	{
	case EBehaviorType::Attack:
		InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);
		InputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayableCharacter::onMobClickedAttack);   //바인딩 함수 변경
		Attack();
		break;
	case EBehaviorType::Defense:
		InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);
		Defense();
		break;
	case EBehaviorType::Capture:
		InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);
		InputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayableCharacter::onMobClickedCapture);   //바인딩 함수 변경
		Capture();
		break;
	default:
		break;
	}
}

void APlayableCharacter::Attack()                       //몹이 죽을때, battleActors - 1(플레이어)- 펫 숫자 ==0 이면 배틀 종료 
{
	UE_LOG(LogTemp, Log, TEXT("Player Attack"));
	behaviorType = EBehaviorType::Idle;
}

void APlayableCharacter::Defense()              //방어를 했으면 데미지를 받을때 적게 받는다.  데미지의 값을 줄임
{
	UE_LOG(LogTemp, Log, TEXT("Player Defense"));
	playerDefensePower += 1;						//1만큼 방어력 증가
	behaviorType = EBehaviorType::Idle;
	turnEnd(battleActors, battleActors_index);
}

void APlayableCharacter::Capture()
{
	UE_LOG(LogTemp, Log, TEXT("Player Capture"));
	behaviorType = EBehaviorType::Idle;
	//turnEnd(battleActors, battleActors_index);
}

void APlayableCharacter::MouseClicktoMove()         //배틀 상태가 아닐때 마우스 클릭한 위치로 이동하는 함수
{
	FHitResult hitResult;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController)
	{
		// 마우스 클릭 위치 가져오기
		FVector MouseLocation, MouseDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

		const FVector Start = MouseLocation;
		const FVector End = Start + MouseDirection * 50000.0f;
		//UE_LOG(LogTemp, Log, TEXT("%s"), *End.ToString());

		GetWorld()->LineTraceSingleByChannel(hitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (hitResult.bBlockingHit)
		{
			FRotator PlayerRot = RotationCorrection(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), hitResult.Location));
			PlayerRot.Pitch = 0.0f;
			GetMesh()->SetWorldRotation(PlayerRot);
			FVector Destination = hitResult.Location;
			//SetActorRotation(PlayerRot);
			Destination.Z = GetActorLocation().Z; // z 좌표를 현재 위치의 높이로 설정

			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, Destination);
		}
	}
}
void APlayableCharacter::onMobClickedAttack()         //마우스 클릭한 위치로 이동하는 함수
{                                               //클릭시 바인딩 되는 함수를 통째로 바꾸는것이 맞은가, 레이캐스트는 어차피 쏘니까 그 아래 행동만 다르게 하는게 맞는가
	FHitResult hitResult;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController)
	{
		
		// 마우스 클릭 위치 가져오기
		FVector MouseLocation, MouseDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

		const FVector Start = MouseLocation;
		const FVector End = Start + MouseDirection * 50000.0f;
		//UE_LOG(LogTemp, Log, TEXT("%s"), *End.ToString());

		GetWorld()->LineTraceSingleByChannel(hitResult, Start, End, ECollisionChannel::ECC_Visibility);
		
		//
		if (hitResult.GetActor()->IsA(AMobCharacter::StaticClass()))           //클릭한게 몹이면       //오브젝트를 클릭하는 경우도 생각해야됨
		{
			UE_LOG(LogTemp, Warning, TEXT("isMob"));
			AMobCharacter* TargetActor = Cast<AMobCharacter>(hitResult.GetActor());
			TargetActor->TakeDamage(playerAttackPower);				//데미지 주기

			if (TargetActor->mobHealthPoint <= 0)                                   //때린게 피가 0이되면
			{

				battleActors.Remove(TargetActor);                                           //배열에서 제거하고
				TargetActor->Destroy();                                                     //삭제 ->죽는모션으로 대체

				if (battleActors.Num() - 1 - myPets.Num() == 0)                //if(battleActors - 1(플레이어)- 펫 숫자 ==0) 이면(남은 적이 없으면)
				{                                               //배틀종료
					isInBattle = false;
					BattleEnd();
					PlayPunchMontage(1.0f);

				}
				else
				{
					InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);       //공격 후 바인딩 초기화해서 재공격 막기
					PlayPunchMontage(1.0f);					//배틀종료가 아닌 턴종료 는 Anim 안에

				}
			}
			else
			{
				InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);       //공격 후 바인딩 초기화해서 재공격 막기
				PlayPunchMontage(1.0f);					//배틀종료가 아닌 턴종료 는 Anim 안에

			}



		}



	}
}
void APlayableCharacter::IsPunchAnimationPlaying()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance->Montage_IsPlaying(PunchMontage))
	{
		GetWorldTimerManager().ClearTimer(AnimationHandle);
		if (!isInBattle)
		{
			BattleEnd();
		}
		else
			turnEnd(battleActors, battleActors_index);
	}
}
void APlayableCharacter::IsAttackedAnimationPlaying()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance->Montage_IsPlaying(AttackedMontage))
	{
		GetWorldTimerManager().ClearTimer(AnimationHandle);
		//turnEnd(battleActors, battleActors_index);
	}
}
void APlayableCharacter::PlayPunchMontage(float PlayRate)
{
	PlayAnimMontage(PunchMontage, PlayRate);
	GetWorldTimerManager().SetTimer(AnimationHandle, this, &APlayableCharacter::IsPunchAnimationPlaying, 0.1f, true);

}
void APlayableCharacter::PlayAttackedMontage(float PlayRate)
{
	PlayAnimMontage(AttackedMontage, PlayRate);
	GetWorldTimerManager().SetTimer(AnimationHandle, this, &APlayableCharacter::IsAttackedAnimationPlaying, 0.1f, true);

}
FRotator APlayableCharacter::RotationCorrection(FRotator rot)
{
	rot.Yaw += -90.0f;
	return rot;
}
void APlayableCharacter::GameOver()
{
	WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverView);
	WidgetInstance->AddToViewport();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

}
void APlayableCharacter::BattleEnd()
{
	for (ABattleCharacter* pet : myPets)
	{
		Cast<APet_AIController>(pet->GetController())->IsNotBattle();
	}
	UE_LOG(LogTemp, Warning, TEXT("battleEnd"));
	WidgetInstance->RemoveFromViewport();                                     //UI지우고
	isInBattle = false;							//배틀에서 벗어나
	battleActors.Reset();
	Cast<APlayerController>(GetController())->SetViewTarget(this);
	InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);           //바인딩 초기화
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayableCharacter::MouseClicktoMove);   //움직이는걸로
}

void APlayableCharacter::onMobClickedCapture()         //마우스 클릭한 위치로 이동하는 함수
{                                               //클릭시 바인딩 되는 함수를 통째로 바꾸는것이 맞은가, 레이캐스트는 어차피 쏘니까 그 아래 행동만 다르게 하는게 맞는가
	FHitResult hitResult;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController)
	{
		// 마우스 클릭 위치 가져오기
		FVector MouseLocation, MouseDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

		const FVector Start = MouseLocation;
		const FVector End = Start + MouseDirection * 50000.0f;
		//UE_LOG(LogTemp, Log, TEXT("%s"), *End.ToString());

		GetWorld()->LineTraceSingleByChannel(hitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (hitResult.GetActor()->IsA(AMobCharacter::StaticClass()))           //선택한게 몹이면
		{

			AMobCharacter* TargetActor = Cast<AMobCharacter>(hitResult.GetActor());

			if (CalculateCapturePosibility(TargetActor->mobHealthPoint, TargetActor->mobMaxHealthPoint))	//실행한 함수 반환값이 참이면//포획성공
			{
				TargetActor->ActorTypeTag = "Pet";
				FVector Location = TargetActor->GetActorLocation();
				FRotator Rotation = FRotator(0.0f, 90.0f, 0.0f);
				FVector middlePoint = TurnManager->GetActorLocation();
				Location.X = (middlePoint.X - screenSize.X / 4 - screenSize.X / 3) + (((myPets.Num()) % 3) + 1) * screenSize.X / 3;
				Location.Y = (middlePoint.Y - 460 - screenSize.Y / 2) - (screenSize.Y / 2) * ((myPets.Num()) / 3);
				Location.Z = 200.0f;
				for (int i = 0; i < battleActors.Num(); i++)
				{
					if (battleActors[i] == TargetActor)
					{
						TargetActor->Destroy();
						ABattleCharacter* NewPet = GetWorld()->SpawnActor<ABattleCharacter>(PetToSpawn, Location, Rotation);
						Cast<APet_AIController>(NewPet->GetController())->IsBattle();
						battleActors.RemoveAt(i);
						battleActors.Insert(NewPet, i);
						myPets.Add(NewPet);
						//UE_LOG(LogTemp, Warning, TEXT("battleActors.Num(): %d, myPets.Num: %d"), battleActors.Num(), myPets.Num())
					}

				}
				UE_LOG(LogTemp, Log, TEXT("Capture Success"));
			}
			if (battleActors.Num() - 1 - myPets.Num() == 0)                //if(battleActors - 1(플레이어)- 펫 숫자 ==0) 이면(남은 적이 없으면)
			{                                               //배틀종료
				BattleEnd();
			}
			else
			{
				InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);       //공격 후 바인딩 초기화해서 재공격 막기
				turnEnd(battleActors, battleActors_index);       //배틀종료가 아닌 턴종료
			}

		}
	}
}
bool APlayableCharacter::CalculateCapturePosibility(float mobHP, float mobMaxHP)
{
	int randomNumber = rand() % 100;
	if (mobHP <= mobMaxHP * 0.1f)
	{
		if (randomNumber < capturePosibility + 30)
		{
			//포획

			return true;
		}
		else
		{
			//포획 실패
			return false;
		}

	}
	else if (mobHP <= mobMaxHP * 0.3f)
	{
		if (randomNumber < capturePosibility + 20)
		{
			//포획
			return true;
		}
		else
		{
			//포획 실패
			return false;
		}

	}
	else if (mobHP <= mobMaxHP * 0.5f)
	{
		if (randomNumber < capturePosibility + 10)
		{
			//포획
			return true;
		}
		else
		{
			//포획 실패
			return false;
		}

	}
	else
	{
		if (randomNumber < capturePosibility)
		{
			//포획
			return true;
		}
		else
		{
			//포획 실패
			return false;
		}
	}
}

void APlayableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)        //몹이랑 부딪히면 //배틀 시작
{

	//UE_LOG(LogTemp, Log, TEXT("NotifyActorBeginOverlap"));
	if (true && !isInBattle)               //OtherActor.collider == 몹이면
	{

		//UE_LOG(LogTemp, Warning, TEXT("intheBattle"));
		isInBattle = true;

		battleActors.Add(this);
		for (int i = 0; i < myPets.Num(); i++)
		{
			battleActors.Add(myPets[i]);
		}
		if (OtherActor)
		{
			battleActors.Add(Cast<ABattleCharacter>(OtherActor));    //부딪힌거 담고
			//CreatePet();
			CreateChildMob();					// 자녀 넣기									//포함된거 담고
		}

		if (!WidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set in the character!"));
			return;
		}
		if (!WidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("widget instance!"));
			WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		}

		// 이미 위젯이 생성되어 있다면 리턴
		/*if (WidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("WidgetInstance is in the character!"));
			return;
		}*/

		// 화면에 추가
		WidgetInstance->AddToViewport();


		TurnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));     //턴매니저 가져오고

		TurnManager->SetActorLocation(OtherActor->GetActorLocation());		//가운데인 턴매니저 기준으로 x는 300 y는 660

		Cast<APlayerController>(GetController())->SetViewTarget(TurnManager);		//카메라 시점 턴매니저로




		UGameViewportClient* ViewportClient = GEngine->GameViewport;
		ViewportClient->GetViewportSize(screenSize);

		FVector middlePoint = OtherActor->GetActorLocation();

		FVector playerPoint;
		FVector petPoint;
		FVector mobPoint;
		//mobPoint.X = middlePoint.X - screenSize.X / 4 - screenSize.X / 3;
		//mobPoint.Y = middlePoint.Y + screenSize.Y / 2;
		int mobCount = 0;
		int petCount = 0;

		for (ABattleCharacter* target : battleActors)					//battleActor 위치 정하기
		{

			if (target->ActorTypeTag == "Player")
			{
				target->FindComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();			//Cliked move 때문에 이동하는걸 막기위해 넣음.
				this->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
				playerPoint.X = middlePoint.X + 150;
				playerPoint.Y = middlePoint.Y - 460;
				playerPoint.Z = target->GetActorLocation().Z + 10;

				target->SetActorLocation(playerPoint);
				target->GetMesh()->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));		//플레이어는 mesh를 돌려야됨
				UE_LOG(LogTemp, Warning, TEXT("Rotate"));
				target->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));

			}
			else if (target->ActorTypeTag == "Mob")		//몹이면
			{
				target->FindComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
				mobPoint.X = (middlePoint.X - screenSize.X / 4 - screenSize.X / 3) + ((mobCount % 3) + 1) * screenSize.X / 3;
				mobPoint.Y = (middlePoint.Y + screenSize.Y / 2) + (screenSize.Y / 2) * (mobCount / 3);
				mobPoint.Z = 200.0f;
				Cast<AMob_AIController>(target->GetController())->IsBattle();			//배틀돌입 멈춰라
				target->SetActorLocation(mobPoint);
				target->SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
				mobCount++;
			}
			else if (target->ActorTypeTag == "Pet")
			{
				target->FindComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
				petPoint.X = (middlePoint.X - screenSize.X / 4 - screenSize.X / 3) + ((petCount % 3) + 1) * screenSize.X / 3;
				petPoint.Y = (middlePoint.Y - 460 - screenSize.Y / 2) - (screenSize.Y / 2) * (petCount / 3);
				petPoint.Z = 200.0f;
				Cast<APet_AIController>(target->GetController())->IsBattle();
				target->SetActorLocation(petPoint);
				target->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
				petCount++;
			}

		}


		if (TurnManager)
		{
			//UE_LOG(LogTemp, Log, TEXT("TurnManager is in"));
			//TurnManager->StartBattleTimer(CollidingActors);          //배틀모드 진입
			// 

			InputComponent->RemoveActionBinding("LeftClick", IE_Pressed);

			TurnManager->nextTurn(battleActors, 0);
		}
	}
}
void APlayableCharacter::CreatePet()
{
	//UE_LOG(LogTemp, Warning, TEXT("CreatePet"));
	for (ABattleCharacter* pet : myPets)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
		ABattleCharacter* NewMobCharacter = GetWorld()->SpawnActor<ABattleCharacter>(PetToSpawn, SpawnLocation, SpawnRotation);

	}
}

void APlayableCharacter::CreateChildMob()
{
	//UE_LOG(LogTemp, Warning, TEXT("CreateChildMob"));
	int randChildNum = FMath::RandRange(0, 5);
	for (int i = 0; i < randChildNum; i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Create : %d"), i);
		FVector SpawnLocation = battleActors[1]->GetActorLocation();  // 적절한 위치값으로 설정
		SpawnLocation.X += 500 * (i + 1);
		SpawnLocation.Z += 500 * (i + 1);
		FRotator SpawnRotation = battleActors[1]->GetActorRotation();// 적절한 회전값으로 설정

		//TSubclassOf<AMobCharacter> ActorClass = AMobCharacter::StaticClass();
		// 새로운 AMobCharacter 객체 생성


		// = 소환은 배틀모드 진입시 소환

		ABattleCharacter* NewMobCharacter = GetWorld()->SpawnActor<ABattleCharacter>(MobToSpawn, SpawnLocation, SpawnRotation);


		battleActors.Add(NewMobCharacter);
	}
}



// Called when the game starts or when spawned
void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	//PlayAttackedMontage(1.0f);
	//GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetActorTransform());
	this->FindComponentByClass<UCharacterMovementComponent>()->StopMovementImmediately();
}

// Called every frame
void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayableCharacter::MouseClicktoMove);

}

