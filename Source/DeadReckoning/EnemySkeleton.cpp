// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkeleton.h"

#include "DRPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"

// Sets default values
AEnemySkeleton::AEnemySkeleton()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetMesh(), "RightHandSocket");

	Health = CreateDefaultSubobject<UHealthComponent>("Health");
	
	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>("HealthBarWidgetComp");
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

// Called when the game starts or when spawned
void AEnemySkeleton::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ADRPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	check(Player != nullptr);
}

// Called every frame
void AEnemySkeleton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackRequestedTimer -= DeltaTime;
	KnockbackTimer -= DeltaTime;

	if (bTraceAttack) PerformAttackTrace();
}

void AEnemySkeleton::PerformAttackTrace()
{
	FVector SwordBase = SwordMesh->GetSocketLocation(TEXT("Base"));
	FVector SwordTip = SwordMesh->GetSocketLocation(TEXT("Tip"));
	// DrawDebugLine(GetWorld(), SwordBase, SwordTip, FColor::Red, false, 3);
	TArray<FHitResult> SwordHits;
	GetWorld()->LineTraceMultiByChannel(SwordHits, SwordBase, SwordTip, ECC_Pawn);
	for (FHitResult HitResult : SwordHits)
	{
		if (HitResult.GetActor() == nullptr || HitResult.GetActor() != Player) continue;

		FVector SkeleToPlayerDir = (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();

		Player->TakeKnockback(AttackKnockbackDuration, SkeleToPlayerDir * AttackKnockbackStrength);

		// ApplyDamage HAS to be called at the end of this logic in case it destroys the enemy
		UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
	}
}

void AEnemySkeleton::TryAttack()
{
	AttackRequestedTimer = 0.2f;
	if (KnockbackTimer <= 0.f) SetIsAttacking(true);
}

void AEnemySkeleton::SetIsAttacking(const bool InIsAttacking)
{
	bIsAttacking = InIsAttacking;
	if (bIsAttacking) GetCharacterMovement()->MaxWalkSpeed = AttackingWalkSpeed;
	else GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AEnemySkeleton::SetTraceAttack(const bool bInTraceAttack)
{
	bTraceAttack = bInTraceAttack;
}


void AEnemySkeleton::TakeKnockback(const float& KnockbackDuration, const FVector& Impulse)
{
	KnockbackTimer = KnockbackDuration;
	bTraceAttack = false;
	GetCharacterMovement()->AddImpulse(Impulse, true); //bVelocityChange makes impulse ignore the mass
}

void AEnemySkeleton::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Skeleton die!!!!!"));

	Destroy();
}
