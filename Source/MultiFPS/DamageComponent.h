// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIFPS_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float CurHealth;
	
public:	
	// Sets default values for this component's properties
	UDamageComponent();

	void TakeDamage(float Damage, AActor* DamageCauser);

	void SetCurHealth(float Health);
	float GetCurHealth();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};