// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayableCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	PlayableCharacter = Cast<APlayableCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PlayableCharacter == nullptr)
		PlayableCharacter = Cast<APlayableCharacter>(TryGetPawnOwner());
}