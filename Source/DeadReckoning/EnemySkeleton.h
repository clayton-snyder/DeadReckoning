// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "EnemySkeleton.generated.h"

UCLASS()
class DEADRECKONING_API AEnemySkeleton : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Combat")
	USkeletalMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UHealthComponent* Health;

public:
	// Sets default values for this character's properties
	AEnemySkeleton();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void TryAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void SetIsAttacking(const bool InIsAttacking);

	UFUNCTION(BlueprintCallable)
	void SetTraceAttack(const bool bInTraceAttack);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void TakeKnockback(const float& KnockbackDuration, const FVector &Impulse);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// STATE
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bTraceAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackRequestedTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float KnockbackTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
	bool KnockbackRefreshAnim = false;


	// STATS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackDamage  = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackKnockbackDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float AttackKnockbackStrength = 100000.f;
	
	float MaxWalkSpeed = 400.f;
	float AttackingWalkSpeed = 100.f;

private:
	class ADRPlayer* Player;
	
	void PerformAttackTrace();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
