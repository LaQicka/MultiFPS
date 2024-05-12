// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageComponent.h"
#include "MultiFPSCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "TurretActor.generated.h"

UCLASS()
class MULTIFPS_API ATurretActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurretActor();

	UPROPERTY(EditAnywhere)
	UDamageComponent* DamageComponent;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* LookSphere;

	UPROPERTY(EditAnywhere)
	USceneComponent* ForwardRef;

	UPROPERTY(EditAnywhere)
	float CurrentHealth = 100;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	UPROPERTY(EditAnywhere)
	float TrackingSpeed;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AMultiFPSCharacter* TrackingPlayer = nullptr;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void GetNearestPlayerPawn();
	bool HasDirectLineOfSightToActor(AActor* TargetActor) const;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
