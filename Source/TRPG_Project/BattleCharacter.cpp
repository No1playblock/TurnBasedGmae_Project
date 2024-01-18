// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleCharacter::doTurn(TArray<ABattleCharacter*> CollidingActors, int index)
{
}

void ABattleCharacter::TakeDamage(int atk)
{
}
