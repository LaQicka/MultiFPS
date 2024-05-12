// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"

// Sets default values for this component's properties
UDamageComponent::UDamageComponent(): CurHealth(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UDamageComponent::TakeDamage(float Damage, AActor* DamageCauser)
{
// 	UE_LOG(LogTemp, Warning, TEXT("DAMAGE COMP %f"), CurHealth)
// 	if(CurHealth <= 0)
// 	{
// 		return;
// 	}
//
// 	CurHealth -= Damage;
//
// 	if(CurHealth <= 0)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("ACTOR DEAD"));
// 	}
}

void UDamageComponent::SetCurHealth(float Health)
{
	CurHealth = Health;
}

float UDamageComponent::GetCurHealth()
{
	return CurHealth;
}


// Called when the game starts
void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

