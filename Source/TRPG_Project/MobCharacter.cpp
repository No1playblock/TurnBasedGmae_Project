// Fill out your copyright notice in the Description page of Project Settings.


#include "MobCharacter.h"
#include "NavigationSystem.h"

#include "PlayableCharacter.h"
#include "PetCharacter.h"
#include "TurnManager.h"


// Sets default values
AMobCharacter::AMobCharacter() :
    mobHealthPoint(50.0f),
    mobMaxHealthPoint(100.0f),
    mobAttackPower(5.0f),
    mobDefensePower(0.0f)
{
    int randChildNum = FMath::RandRange(0, 5);

    PrimaryActorTick.bCanEverTick = true;
    ActorTypeTag = "Mob";
    behaviorType = EBehaviorType::Idle;

}

// Called when the game starts or when spawned
void AMobCharacter::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AMobCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AMobCharacter::doTurn(TArray<ABattleCharacter*> CollidingActors, int index)
{
    UE_LOG(LogTemp, Warning, TEXT("MobdoTurn"));
    battleActors = CollidingActors;
    battleActors_index = index;
    for (ABattleCharacter* target : CollidingActors)                      //턴에 진입할때마다 액터 객체 확인
    {
        //if ()                    //battleactor 상속으로 actortypeTag 확인으로 교체

        //if(target->IsA(APlayableCharacter::StaticClass()))          //여기서 오류
        if (target->ActorTypeTag == "Player" || target->ActorTypeTag == "Pet")
        {
            //UE_LOG(LogTemp, Warning, TEXT("playerAdd"));
            TargetActors.Add(target);
            //UE_LOG(LogTemp, Warning, TEXT("%s"), *target->ActorTypeTag);
        }

    }

    EBehaviorType randomNumber = static_cast<EBehaviorType>(FMath::RandRange(1, 2));
    int randNum = FMath::RandRange(0, TargetActors.Num() - 1);              //랜덤으로  뽑고
    switch (randomNumber)
    {
    case EBehaviorType::Attack:
        //Attack(CollidingActors, index);
        UE_LOG(LogTemp, Log, TEXT("Mob Attack"));

        if (TargetActors[randNum]->ActorTypeTag == "Player")                 //뽑은게 플레이어면
        {
            APlayableCharacter* player = Cast<APlayableCharacter>(TargetActors[randNum]);
            PlayPunchMontage(1.0f);
            player->TakeDamage(mobAttackPower);        //플레이어 TakeDamage
            if (player->playerHealthPoint <= 0.0f)
            {
                isGameOver = true;
                player->playerHealthPoint = 0.0f;
                player->GameOver();
                UE_LOG(LogTemp, Log, TEXT("GameOver"));
            }
            else
            {
                //PlayPunchMontage(1.0f); 
            }

        }
        else if (TargetActors[randNum]->ActorTypeTag == "Pet")                 //뽑은게 플레이어면
        {
            APetCharacter* pet = Cast<APetCharacter>(TargetActors[randNum]);
            PlayPunchMontage(1.0f);
            pet->TakeDamage(mobAttackPower);        //플레이어 TakeDamage
            if (pet->petHealthPoint <= 0.0f)
            {
                pet->petHealthPoint = 0.0f;
                CollidingActors.Remove(TargetActors[randNum]);
                pet->Destroy();
            }
        }

        //turnEnd(CollidingActors, index);
        behaviorType = EBehaviorType::Idle;
        break;
    case EBehaviorType::Defense:
        Defense();
        turnEnd(CollidingActors, index);
        break;
    }
}
void AMobCharacter::turnEnd(TArray<ABattleCharacter*> CollidingActors, int index)
{
    TargetActors.Empty();
    TurnManager->nextTurn(CollidingActors, index);
}
void AMobCharacter::TakeDamage(int atk)
{
    mobHealthPoint -= (atk + mobDefensePower);
    PlayAttackedMontage(1.0f);
    UE_LOG(LogTemp, Warning, TEXT("Mob HP: %f"), mobHealthPoint);


}
void AMobCharacter::Attack(TArray<ABattleCharacter*> CollidingActors, int index)            //플레이어 HP가 0인지 검사하고 0이면 배틀종료
{


}
void AMobCharacter::Defense()
{
    UE_LOG(LogTemp, Log, TEXT("Mob Defense"));
    mobDefensePower += 1;
    behaviorType = EBehaviorType::Idle;
    //TurnManager->nextTurn(CollidingActors, index);
}



// Called to bind functionality to input
void AMobCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMobCharacter::IsPunchAnimationPlaying()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance->Montage_IsPlaying(PunchMontage))
    {
        GetWorldTimerManager().ClearTimer(AnimationHandle);
        turnEnd(battleActors, battleActors_index);
    }
}
void AMobCharacter::IsAttackedAnimationPlaying()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackedMontage))              //
    {
        GetWorldTimerManager().ClearTimer(AnimationHandle);
        //turnEnd(battleActors, battleActors_index);
    }
}
void AMobCharacter::PlayPunchMontage(float PlayRate)
{
    PlayAnimMontage(PunchMontage, PlayRate);
    GetWorldTimerManager().SetTimer(AnimationHandle, this, &AMobCharacter::IsPunchAnimationPlaying, 0.1f, true);

}
void AMobCharacter::PlayAttackedMontage(float PlayRate)
{
    PlayAnimMontage(AttackedMontage, PlayRate);
    GetWorldTimerManager().SetTimer(AnimationHandle, this, &AMobCharacter::IsAttackedAnimationPlaying, 0.1f, true);

}

//void AMobCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
//{
//	UE_LOG(LogTemp, Log, TEXT("NotifyActorBeginOverlap"));
//}

