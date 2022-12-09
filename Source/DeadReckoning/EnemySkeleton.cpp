// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkeleton.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemySkeleton::AEnemySkeleton()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetMesh(), "RightHandSocket");

	Health = CreateDefaultSubobject<UHealthComponent>("Health");

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

// Called when the game starts or when spawned
void AEnemySkeleton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySkeleton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackRequestedTimer -= DeltaTime;
	KnockbackTimer -= DeltaTime;

	if (bIsAttacking)
	{
		DrawDebugLine(GetWorld(), SwordMesh->GetSocketLocation(TEXT("Base")), SwordMesh->GetSocketLocation(TEXT("Tip")),
					  FColor::Red, false, 3, 0, 1);
	}
}

void AEnemySkeleton::SetIsAttacking(const bool InIsAttacking)
{
	UE_LOG(LogTemp, Warning, TEXT("SKELe: Set IsAttacking from %s to %s"),
		   (bIsAttacking ? TEXT("TRUE") : TEXT("FALSE")), (InIsAttacking) ? TEXT("TRUE") : TEXT("FALSE"));
	bIsAttacking = InIsAttacking;
	if (bIsAttacking) GetCharacterMovement()->MaxWalkSpeed = AttackingWalkSpeed;
	else GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AEnemySkeleton::SetTraceAttack(const bool bInTraceAttack)
{
	bTraceAttack = bInTraceAttack;
}


void AEnemySkeleton::TakeKnockback(const float& KnockbackDuration, const FVector &Impulse)
{
	KnockbackTimer = KnockbackDuration;
	GetCharacterMovement()->AddImpulse(Impulse, true); //bVelocityChange makes impulse ignore the mass
}

void AEnemySkeleton::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Skeleton die!!!!!"));

	Destroy();
}
