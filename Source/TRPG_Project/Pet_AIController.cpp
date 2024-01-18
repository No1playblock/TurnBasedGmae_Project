// Fill out your copyright notice in the Description page of Project Settings.


#include "Pet_AIController.h"
#include "NavigationSystem.h"
#include "PetCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

APet_AIController::APet_AIController()
{
	//UE_LOG(LogTemp, Log, TEXT("constructor"));

	RepeatInterval = 0.6f;

}
void APet_AIController::MoveToPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("FOLLOW"));
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();

	SetFocus(PlayerController->GetPawn());
	MoveToLocation(PlayerLocation);

}

void APet_AIController::OnPossess(APawn* InPawn)		//pawn's name : FPBossCharacter
{
	Super::OnPossess(InPawn);
	//UE_LOG(LogTemp, Warning, TEXT("Possess"));
	APetCharacter* possessedPawn = Cast<APetCharacter>(InPawn);
	StartLocation = InPawn->GetActorLocation();

	IsNotBattle();
}
void APet_AIController::IsBattle()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}
void APet_AIController::IsNotBattle()
{
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &APet_AIController::MoveToPlayer, RepeatInterval, true);
}
void APet_AIController::OnUnPossess()
{
	Super::OnUnPossess();
	IsBattle();
}