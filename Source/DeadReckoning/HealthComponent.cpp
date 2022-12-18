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

	// Populate the appropriate pointer based on the type of the owner. This method is a little clunky but it's less
	// cycles than casting each time damage is taken, and creating specific HealthComponents for each actor type is
	// too over-engineered for this scope
	Player = Cast<ADRPlayer>(GetOwner());
	if (Player != nullptr) return;

	Skele = Cast<AEnemySkeleton>(GetOwner());
	if (Skele != nullptr) return;

	// If we get here, we haven't coded for a Character type that we attached this component to, which should be fixed.
	UE_LOG(LogTemp, Fatal, TEXT("GetOwner() did not cast to any expected Character type in HealthComponent."));
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                   AController* Instigator, AActor* DamageCauser)
{
	if (Player != nullptr)
	{
		if (Player->GetFlashTime() > 0.f)
		{
			return;
		}
		CurrHealth -= Damage;
		if (CurrHealth <= 0.f)
		{
			Player->Die();
			return;
		}
		Player->RefreshFlashTime();
	}
	else if (Skele != nullptr)
	{
		CurrHealth -= Damage;
		Skele->UpdateHealthBar(CurrHealth, MaxHealth);
		if (CurrHealth <= 0.f)
		{
			Skele->Die();
			return;
		}
	}
	else UE_LOG(LogTemp, Fatal, TEXT("In HealthComponent, both Player and Skele are nullptrs!"));
}
