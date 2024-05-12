// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagableCube.h"

// Sets default values
ADamagableCube::ADamagableCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 100;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetSimulatePhysics(true);
	
	CurrentHealth = MaxHealth;
	
}

// Called when the game starts or when spawned
void ADamagableCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamagableCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ADamagableCube::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	if(CurrentHealth <= 0)
	{
		Destroy();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

