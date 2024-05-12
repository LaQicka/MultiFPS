// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageComponent.h"
#include "StatusUserWidget.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_WeaponComponent.h"
#include "MultiFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMultiFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CurrentHealth;
	UPROPERTY(EditAnywhere)
	float MaxHealth;

	// UPROPERTY(EditAnywhere)
	// UStatusUserWidget* StatusWidget;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetWeaponCharge() const;

	UPROPERTY(EditAnywhere)
	UDamageComponent* DamageComp;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* BodyMesh;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category=Projectile)
	TSubclassOf<class AMultiFPSProjectile> ProjectileClass;

public:
	AMultiFPSCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();
	
	USkeletalMeshComponent* GetBodyComponent();

	void Die();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isAlive = true;

	UFUNCTION(BlueprintCallable)
	bool GetIsAlive();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnProjectile(FVector Location, FRotator Rotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrawDebugLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingActor,
											  float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDrawDebugLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingActor,
											  float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass);
	
	
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void SpawnProjectile(FVector Location, FRotator Rotation);
	void RequestDrawLazerLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingActor,
						float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass);

	void DrawLazerLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingPlayer,
		float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass);

	UTP_WeaponComponent* Weapon;
	void StartSprint();
	void StopSprint();
	bool GetAlive();
	
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

