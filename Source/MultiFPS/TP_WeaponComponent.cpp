// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "MultiFPSCharacter.h"
#include "MultiFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

float UTP_WeaponComponent::GetCharge() const
{
	return CurCharge/MaxCharge;
}

void UTP_WeaponComponent::StartCharging()
{
	bIsFiring = true;
}

void UTP_WeaponComponent::StopCharging()
{
	bIsFiring = false;
	CurCharge = 0;
}

void UTP_WeaponComponent::FireLazer()
{
	if(!CurrentWeaponType)
	{
		bIsFiring = true;
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			FVector PlayerViewPointLocation;
			FRotator PlayerViewPointRotation;
			
			PlayerController->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
			
			// Переводим в ротацию в вектор направления
			FVector ShotDirection = PlayerViewPointRotation.Vector();
			// Вычисляем EndLocation для луча, используя направление взгляда и заданную дальность
			FVector EndLocation = PlayerViewPointLocation + (ShotDirection * LazerRange);
			FVector StartLocation = this->GetSocketLocation(TEXT("Muzzle"));

			FHitResult HitResult;
			FCollisionQueryParams TraceParams(FName(TEXT("LaserTrace")), true, Character);
			TraceParams.bTraceComplex = true; // Установка в true, если нужно более точное определение столкновений
			TraceParams.bReturnPhysicalMaterial = false; // Установка в true, если нужна информация о физическом материале
			// Результат трассировки
			// Выполнение трассировки луча (raycast)
			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,     // Результат трассировки
				StartLocation,    // Начальная позиция луча
				EndLocation,      // Конечная позиция луча
				ECC_Visibility,   // Канал коллизии (может быть изменен в зависимости от ваших настроек)
				TraceParams       // Параметры трассировки
			);
			
			if (bHit)
			{
				// Обработка столкновения
				AActor* HitActor = HitResult.GetActor();
	    
				if (HitActor && HitActor->CanBeDamaged())
				{
					AController* InstigatedBy = Character->GetInstigatorController();
					UGameplayStatics::ApplyDamage(HitActor,5.0f,InstigatedBy,Character,UDamageType::StaticClass());
				}
				
				// Отрисовываем линию до точки столкновения
				TSubclassOf<UDamageType> DamageTypeClass;
				if(CurCharge>0) Character->RequestDrawLazerLine(StartLocation, HitResult.Location, FColor::Red,
					HitActor, LazerDamage, Character, Character->GetController(), DamageTypeClass);
			}
			else
			{
				// Отрисовываем линию полностью, если ничего не было поражено
				TSubclassOf<UDamageType> DamageTypeClass;
				if(CurCharge>0) Character->RequestDrawLazerLine(StartLocation, EndLocation, FColor::Red,
					nullptr, LazerDamage, Character, Character->GetController(), DamageTypeClass);
			}
		}
	}
}

void UTP_WeaponComponent::StopLazerFire()
{
	if(!CurrentWeaponType)
	{
		bIsFiring = false;
	}
}

void UTP_WeaponComponent::ChangeWeaponType(const FInputActionValue& Value)
{
	CurrentWeaponType = !CurrentWeaponType;
}

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	SetIsReplicatedByDefault(true);
	
	bIsFiring = false;
	MaxCharge = 2;
	CurCharge = 0;
	LazerDamage = 5;
}

void UTP_WeaponComponent::Fire()
{
	StopCharging();
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if(CurrentWeaponType)
	{
		// Try and fire a projectile
		if (ProjectileClass != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
				// Spawn the projectile at the muzzle
				// AMultiFPSProjectile* Projectile = World->SpawnActor<AMultiFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				Character->SpawnProjectile(SpawnLocation,SpawnRotation);
			}
		}	
	}

	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		// UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickWeapon(DeltaTime);
}

void UTP_WeaponComponent::TickWeapon(float DeltaTime)
{
	if(bIsFiring && CurrentWeaponType)CurCharge += DeltaTime;
	else if(bIsFiring && !CurrentWeaponType) CurCharge -= DeltaTime;
	else if(!bIsFiring && !CurrentWeaponType) CurCharge += DeltaTime;
}

void UTP_WeaponComponent::AttachWeapon(AMultiFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr || Character->GetHasRifle())
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	if(Character->IsLocallyControlled())
	{
		AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));	
	}
	else
	{
		AttachToComponent(Character->GetBodyComponent(), AttachmentRules, FName(TEXT("GripPoint")));	
	}
	Character->Weapon = this;
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::FireLazer);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopLazerFire);

			EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartCharging);
			EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Canceled, this, &UTP_WeaponComponent::StopCharging);
			EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::Fire);
			EnhancedInputComponent->BindAction(WeaponChangeAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::ChangeWeaponType);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}
	Character->Weapon = nullptr;
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}


