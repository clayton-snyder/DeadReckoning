// Fill out your copyright notice in the Description page of Project Settings.


#include "DRPlayer.h"

#include "DRConstants.h"
#include "EnemySkeleton.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADRPlayer::ADRPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetMesh(), FName("RightHandSocket"));

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Mouse rotation will only affect camera (but movement will orient character to camera)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.f, 0.0f);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	CameraSpringArm->bUsePawnControlRotation = true; // Makes camera arm rotate based on controller pitch/yaw input
	Camera->bUsePawnControlRotation = false; // Since the arm is rotating, no need for camera itself to rotate
}

// Called when the game starts or when spawned
void ADRPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackRequestedTimer -= DeltaTime;
	KnockbackTimer -= DeltaTime;


	if (bTraceAttack) PerformAttackTrace();
}

void ADRPlayer::PerformAttackTrace()
{
	FVector SwordBase = SwordMesh->GetSocketLocation(TEXT("Base"));
	FVector SwordTip = SwordMesh->GetSocketLocation(TEXT("Tip"));
	// DrawDebugLine(GetWorld(), SwordMesh->GetSocketLocation(TEXT("Base")), SwordMesh->GetSocketLocation(TEXT("Tip")),
	// 			  FColor::Green, false, 3, 0, 1);
	// FVector DebugBox = FVector(30.f, 30.f, 10.f);
	// DrawDebugBox(GetWorld(), SwordBase, DebugBox, FColor::Red, false, 3);
	// DrawDebugBox(GetWorld(), (SwordBase + SwordTip) / 2, DebugBox, FColor::Red, false, 3);
	// DrawDebugBox(GetWorld(), SwordTip, DebugBox, FColor::Red, false, 3);

	TArray<FHitResult> SwordHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(HitByAttackThisTrace);
	FCollisionShape Shape = FCollisionShape::MakeBox(FVector3f(15.f, 15.f, 5.f));
	GetWorld()->SweepMultiByChannel(SwordHits, SwordBase, SwordTip, FQuat::Identity,
	                                UDRConstants::TraceChannelEnemy, Shape, Params);
	// GetWorld()->LineTraceMultiByChannel(SwordHits, SwordBase, SwordTip, UDRConstants::TraceChannelEnemy,
	//                                     Params);

	for (FHitResult HitResult : SwordHits)
	{
		if (HitResult.GetActor() == nullptr) continue; // We don't care about anything without an Actor

		HitByAttackThisTrace.Add(HitResult.GetActor());
		AEnemySkeleton* Skele = Cast<AEnemySkeleton>(HitResult.GetActor());

		if (Skele == nullptr) continue;

		FVector SkeleLoc = HitResult.GetActor()->GetActorLocation();
		FVector PlayerToSkeleDir = (SkeleLoc - GetActorLocation()).GetSafeNormal();

		Skele->TakeKnockback(AttackKnockbackDuration, PlayerToSkeleDir * AttackKnockbackStrength);
		Skele->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(SkeleLoc, GetActorLocation()));

		// ApplyDamage HAS to be called at the end of this logic in case it destroys the enemy
		UGameplayStatics::ApplyDamage(Skele, AttackDamage, GetController(), this, UDamageType::StaticClass());
	}
}

// Called to bind functionality to input
void ADRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(UDRConstants::AxisMoveForward, this, &ADRPlayer::HandleMoveInput);
	PlayerInputComponent->BindAxis(UDRConstants::AxisStrafeRight, this, &ADRPlayer::HandleStrafeInput);
	PlayerInputComponent->BindAxis(UDRConstants::AxisLookUp, this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(UDRConstants::AxisLookRight, this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction(UDRConstants::ActionAttack, IE_Pressed, this, &ADRPlayer::HandleAttackInput);
}

void ADRPlayer::HandleMoveInput(const float InputStrength)
{
	const FRotator ControllerRotation = FRotator(0, Controller->GetControlRotation().Yaw, 0);
	const FVector MoveDir = ControllerRotation.RotateVector(FVector(1.f, 0.f, 0.f));
	AddMovementInput(MoveDir, InputStrength);
}

void ADRPlayer::HandleStrafeInput(float InputStrength)
{
	const FRotator ControllerRotator = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	const FVector MoveDir = ControllerRotator.RotateVector(FVector(0.f, 1.f, 0.f));
	AddMovementInput(MoveDir, InputStrength);
}

void ADRPlayer::HandleAttackInput()
{
	AttackRequestedTimer = 0.2f;
	if (KnockbackTimer <= 0.f) SetIsAttacking(true);
}

void ADRPlayer::SetIsAttacking(const bool bInIsAttacking)
{
	UE_LOG(LogTemp, Warning, TEXT("Set IsAttacking from %s to %s"),
	       (bIsAttacking ? TEXT("TRUE") : TEXT("FALSE")), (bInIsAttacking) ? TEXT("TRUE") : TEXT("FALSE"));
	bIsAttacking = bInIsAttacking;
	if (bIsAttacking) GetCharacterMovement()->MaxWalkSpeed = AttackingWalkSpeed;
	else GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ADRPlayer::SetTraceAttack(const bool bInTraceAttack)
{
	bTraceAttack = bInTraceAttack;

	// Clear the "already hit" list after each slash
	if (!bInTraceAttack)
	{
		HitByAttackThisTrace.Empty();
	}
}


void ADRPlayer::TakeKnockback(const float& KnockbackDuration, const FVector& Impulse)
{
	UE_LOG(LogTemp, Warning, TEXT("Set Player KnockbackTimer to %.1f"), KnockbackDuration);
	KnockbackTimer = KnockbackDuration;
	GetCharacterMovement()->AddImpulse(Impulse, true);
}

void ADRPlayer::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Die!!!!!!!!!!!!"));
}
