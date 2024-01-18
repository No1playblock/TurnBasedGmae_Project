// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "PlayableCharacter.h"
#include "BattleCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MobCharacter.h"
#include "PetCharacter.h"

// Sets default values
ATurnManager::ATurnManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));


    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
    Super::BeginPlay();
    CameraArm->SetupAttachment(RootComponent);
    CameraArm->TargetArmLength = 1500.0f;
    CameraArm->SetRelativeRotation(FRotator(-30.f, 180.f, 0.f));
}


// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
void ATurnManager::nextTurn(TArray<ABattleCharacter*> CollidingActors, int index)
{
    ABattleCharacter* NextActor = Cast<ABattleCharacter>(CollidingActors[index]);

    if (CollidingActors.Num() - 1 == index)           //한바퀴종료
    {
        index = 0;

    }
    else index++;                                   //충돌때 0을 받아와서 coliderActors의 개수와 비교해가면서 인덱스와 colliderActors의 개수가 같아 질때까지 1씩증가

    if (index == 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("---------------------"));
    }

    if (NextActor->ActorTypeTag == "Player")      //해당 타입 검사후
    {
        Cast<APlayableCharacter>(NextActor)->doTurn(CollidingActors, index);      //해당타입의 doTurn 실행
    }
    else if (NextActor->ActorTypeTag == "Mob")
    {
        // 몬스터에 대한 처리
        Cast<AMobCharacter>(NextActor)->doTurn(CollidingActors, index);
    }
    else if (NextActor->ActorTypeTag == "Pet")
    {
        Cast<APetCharacter>(NextActor)->doTurn(CollidingActors, index);
    }

}

