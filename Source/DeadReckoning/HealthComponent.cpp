// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "DRPlayer.h"
#include "EnemySkeleton.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false; //TODO: DO WE NEED THIS ON?
	
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrHealth = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                   AController* Instigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Display, TEXT("%s HealthComponent taking %.1f Damage from %s"), *GetOwner()->GetActorNameOrLabel(),
	       Damage, *DamageCauser->GetActorNameOrLabel())
	CurrHealth -= Damage;
	if (CurrHealth <= 0.f)
	{
		AEnemySkeleton* Skele = Cast<AEnemySkeleton>(GetOwner());
		if (Skele != nullptr)
		{
			Skele->Die();
			return;
		}

		ADRPlayer* Player = Cast<ADRPlayer>(GetOwner());
		if (Player != nullptr)
		{
			Player->Die();
			return;
		}
	}
}
