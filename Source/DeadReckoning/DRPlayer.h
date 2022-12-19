// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "DRPlayer.generated.h"

UCLASS()
class DEADRECKONING_API ADRPlayer : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Combat")
	USkeletalMeshComponent* SwordMesh;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	UChildActorComponent* TorchBP;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UHealthComponent* Health;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	class UCameraComponent* Camera;

public:
	// Sets default values for this character's properties
	ADRPlayer();
	
	UFUNCTION(BlueprintCallable)
	void SetIsAttacking(const bool InIsAttacking);

	UFUNCTION(BlueprintCallable)
	void SetTraceAttack(const bool bInTraceAttack);

	UFUNCTION(BlueprintCallable)
	void TakeKnockback(const float &KnockbackDuration, const FVector &Impulse);

	UFUNCTION(BlueprintCallable)
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void HandleMoveInput(float InputStrength);
	void HandleStrafeInput(float InputStrength);
	void HandleAttackInput();
	void EnableTorchPlaceScan();
	void DisableTorchPlaceScan();


	// STATE 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bTraceAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackRequestedTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float KnockbackTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float FlashTime = 1.5f;
	
	// Used to prevent double-hits from one swing
	TArray<AActor*> HitByAttackThisTrace;
	
	bool bScanForTorchPlacement = false;
	float TorchPlaceDist = 500.f;
	void PerformTorchPlaceScan();


	// STATS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackDamage  = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackKnockbackDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackKnockbackStrength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float FlashTimeAfterTakingHit = 3.0f;

	float MaxWalkSpeed = 600.f;
	float AttackingWalkSpeed = 100.f;

private:
	void PerformAttackTrace();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	float GetFlashTime();

	// Set FlashTime to the default FlashTime for the character
	void RefreshFlashTime();

};
