// Fill out your copyright notice in the Description page of Project Settings.


#include "PetCharacter.h"
#include "MobCharacter.h"
#include "NavigationSystem.h"
#include "PlayableCharacter.h"
#include "TurnManager.h"


APetCharacter::APetCharacter() :
    petHealthPoint(100.0f),
    petMaxHealthPoint(100.0f),
    petAttackPower(10.0f),
    petDefensePower(0.0f)
{
    PrimaryActorTick.bCanEverTick = true;
    ActorTypeTag = "Pet";
    behaviorType = EBehaviorType::Idle;
}

// Called when the game starts or when spawned
void APetCharacter::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void APetCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
void APetCharacter::IsPunchAnimationPlaying()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance->Montage_IsPlaying(PunchMontage))
    {
        GetWorldTimerManager().ClearTimer(AnimationHandle);
        turnEnd(battleActors, battleActors_index);
    }
}
void APetCharacter::IsAttackedAnimationPlaying()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance->Montage_IsPlaying(AttackedMontage))
    {
        GetWorldTimerManager().ClearTimer(AnimationHandle);
        //turnEnd(battleActors, battleActors_index);
    }
}
void APetCharacter::PlayPunchMontage(float PlayRate)
{
    PlayAnimMontage(PunchMontage, PlayRate);
    GetWorldTimerManager().SetTimer(AnimationHandle, this, &APetCharacter::IsPunchAnimationPlaying, 0.1f, true);

}
void APetCharacter::PlayAttackedMontage(float PlayRate)
{
    PlayAnimMontage(AttackedMontage, PlayRate);
    GetWorldTimerManager().SetTimer(AnimationHandle, this, &APetCharacter::IsAttackedAnimationPlaying, 0.1f, true);

}
void APetCharacter::doTurn(TArray<ABattleCharacter*> CollidingActors, int index)
{
    battleActors = CollidingActors;
    battleActors_index = index;
    pet = 0;
    UE_LOG(LogTemp, Warning, TEXT("petdoTurn"));
    for (ABattleCharacter* target : CollidingActors)                      //턴에 진입할때마다 액터 객체 확인
    {
        if (target->ActorTypeTag == "Mob")
        {
            //UE_LOG(LogTemp, Warning, TEXT("MobAdd"));
            TargetActors.Add(target);
        }
        else if (target->ActorTypeTag == "Player")
        {
            //UE_LOG(LogTemp, Warning, TEXT("PlayerAdd"));
            player = Cast<APlayableCharacter>(target);
        }
        else if (target->ActorTypeTag == "Pet")
        {
            pet++;
        }
    }

    EBehaviorType randomNumber = static_cast<EBehaviorType>(FMath::RandRange(1, 2));
    int randNum = FMath::RandRange(0, TargetActors.Num() - 1);              //랜덤으로  뽑고
    switch (randomNumber)
    {
    case EBehaviorType::Attack:
        //Attack(CollidingActors, index);
        UE_LOG(LogTemp, Log, TEXT("pet Attack"));

        if (TargetActors[randNum]->ActorTypeTag == "Mob")                 //뽑은게 몹이면 (몹만 넣었지만 확인차)
        {
            AMobCharacter* mob = Cast<AMobCharacter>(TargetActors[randNum]);

            mob->TakeDamage(petAttackPower);        //플레이어 TakeDamage
            if (mob->mobHealthPoint <= 0.0f)
            {
                //UE_LOG(LogTemp, Warning, TEXT("Destroy"));
                CollidingActors.Remove(mob);
                mob->Destroy();
                if (CollidingActors.Num() - 1 - pet == 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("BattleEnd"));
                    player->BattleEnd();

                }
                else
                {
                    behaviorType = EBehaviorType::Idle;
                    PlayPunchMontage(1.0f);
                }
            }
            else
            {
                behaviorType = EBehaviorType::Idle;
                PlayPunchMontage(1.0f);

            }
        }
        break;
    case EBehaviorType::Defense:
        Defense();
        turnEnd(CollidingActors, index);
        break;
    }
}
void APetCharacter::turnEnd(TArray<ABattleCharacter*> CollidingActors, int index)
{
    TargetActors.Empty();
    TurnManager->nextTurn(CollidingActors, index);
}
void APetCharacter::TakeDamage(int atk)
{
    petHealthPoint -= (atk + petDefensePower);
    PlayAttackedMontage(1.0f);
    UE_LOG(LogTemp, Warning, TEXT("pet HP: %f"), petHealthPoint);
}
void APetCharacter::Attack(TArray<ABattleCharacter*> CollidingActors, int index)            //플레이어 HP가 0인지 검사하고 0이면 배틀종료
{


}
void APetCharacter::Defense()
{
    UE_LOG(LogTemp, Log, TEXT("pet Defense"));
    petDefensePower += 1;
    behaviorType = EBehaviorType::Idle;
    //TurnManager->nextTurn(CollidingActors, index);
}



// Called to bind functionality to input
void APetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}