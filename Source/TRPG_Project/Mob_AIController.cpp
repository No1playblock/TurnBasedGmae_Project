// Fill out your copyright notice in the Description page of Project Settings.


#include "Mob_AIController.h"
#include "MobCharacter.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


AMob_AIController::AMob_AIController()
{
	UE_LOG(LogTemp, Log, TEXT("constructor"));
	/*BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	check(BlackboardComp);

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	check(BehaviorTreeComp);*/
	RepeatInterval = 3.0f;

}

void AMob_AIController::MovetoRandomPoint()
{
	auto CurrentPawn = GetPawn();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (nullptr == NavSystem) return;	//못 불러왔으면 실패
	FNavLocation RandomLocation;		//내비 기반 좌표찍어놓을 변수

	if (NavSystem->GetRandomPointInNavigableRadius(StartLocation, 500.0f, RandomLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation.Location);
		//UE_LOG(LogTemp, Log, TEXT("Next Location: %s"), *RandomLocation.Location.ToString());
		//ABLOG(Warning, TEXT("Next Location: %s", *RandomLocation.Location.ToString()))
	}
}

void AMob_AIController::OnPossess(APawn* InPawn)		//pawn's name : FPBossCharacter
{
	Super::OnPossess(InPawn);

	AMobCharacter* possessedPawn = Cast<AMobCharacter>(InPawn);
	StartLocation = InPawn->GetActorLocation();


	IsNotBattle();
}
void AMob_AIController::IsBattle()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}
void AMob_AIController::IsNotBattle()
{
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AMob_AIController::MovetoRandomPoint, RepeatInterval, true);
}
void AMob_AIController::OnUnPossess()
{
	Super::OnUnPossess();
	IsBattle();
}
