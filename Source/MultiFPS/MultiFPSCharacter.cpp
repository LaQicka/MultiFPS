// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiFPSCharacter.h"
#include "MultiFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MultiFPSGameMode.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiFPSCharacter

float AMultiFPSCharacter::GetCurrentHealth() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Cur H %f"), CurrentHealth/MaxHealth);
	return CurrentHealth/MaxHealth;
}

float AMultiFPSCharacter::GetWeaponCharge() const
{
	if(Weapon)
	{
		return Weapon->GetCharge();
	}
	return 0;
}

AMultiFPSCharacter::AMultiFPSCharacter()
{
	
	// Character doesnt have a rifle at start
	bHasRifle = false;
	bReplicates = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY MESH"));
	BodyMesh->SetOwnerNoSee(true);
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	
	MaxHealth = 100;
	CurrentHealth = 75;
	
}

void AMultiFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

//////////////////////////////////////////////////////////////////////////// Input

void AMultiFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiFPSCharacter::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMultiFPSCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMultiFPSCharacter::StopSprint);
		
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiFPSCharacter::Look);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiFPSCharacter::ServerDrawDebugLine_Implementation(const FVector& Start, const FVector& End,
	const FColor& Color, AActor* TrackingPlayer, float DamageAmount, AActor* CauserActor, AController* InstigatedBy,
	TSubclassOf<UDamageType> DamageTypeClass)
{
	MulticastDrawDebugLine(Start, End, Color, TrackingPlayer, DamageAmount, CauserActor, InstigatedBy, DamageTypeClass);
}

bool AMultiFPSCharacter::ServerDrawDebugLine_Validate(const FVector& Start, const FVector& End, const FColor& Color,
	AActor* TrackingPlayer, float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass)
{
	return true;
}


// Draw Lazer Logic

void AMultiFPSCharacter::MulticastDrawDebugLine_Implementation(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingActor,
											  float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass)
{
	DrawDebugLine(GetWorld(), Start, End, Color, false, 0.1f, (uint8)'\000', 1.0f);

	UGameplayStatics::ApplyDamage(
				TrackingActor,
				DamageAmount,
				InstigatedBy,
				this,
				DamageTypeClass
			);
}

void AMultiFPSCharacter::RequestDrawLazerLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingActor,
                                              float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		// Мы на клиенте, посылаем запрос на сервер
		ServerDrawDebugLine(Start, End, Color, TrackingActor, DamageAmount, CauserActor, InstigatedBy, DamageTypeClass);
	}
	else
	{
		// Мы на сервере, сразу рисуем линию
		DrawLazerLine(Start, End, Color, TrackingActor, DamageAmount, CauserActor, InstigatedBy, DamageTypeClass);
	}
}

void AMultiFPSCharacter::DrawLazerLine(const FVector& Start, const FVector& End, const FColor& Color, AActor* TrackingPlayer,
		float DamageAmount, AActor* CauserActor, AController* InstigatedBy, TSubclassOf<UDamageType> DamageTypeClass)
{
	MulticastDrawDebugLine(Start, End, Color, TrackingPlayer, DamageAmount, CauserActor, InstigatedBy, DamageTypeClass);
}


void AMultiFPSCharacter::SpawnProjectile(FVector Location, FRotator Rotation)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		// Мы на клиенте, вызываем серверную функцию RPC
		ServerSpawnProjectile(Location,Rotation);
	}
	else
	{
		// Мы на сервере, можем сразу спавнить снаряд
		ServerSpawnProjectile_Implementation(Location, Rotation);
	}
}

void AMultiFPSCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
}

void AMultiFPSCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

bool AMultiFPSCharacter::GetAlive()
{
	return this->isAlive;
}

float AMultiFPSCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                                     AActor* DamageCauser)
{
	if(isAlive)CurrentHealth -= Damage;

	if(CurrentHealth <= 0 && isAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTOR DEAD"));
		isAlive = false;
		Die();
	}
	
	return Damage;
}

void AMultiFPSCharacter::ServerSpawnProjectile_Implementation(FVector Location, FRotator Rotation)
{
	if (GetWorld())
	{
		// Создаём снаряд на сервере
		AMultiFPSProjectile* Projectile = GetWorld()->SpawnActor<AMultiFPSProjectile>(ProjectileClass, Location, Rotation);
	}
}

bool AMultiFPSCharacter::ServerSpawnProjectile_Validate(FVector Location, FRotator Rotation)
{
	return true;
}


bool AMultiFPSCharacter::GetIsAlive()
{
	UE_LOG(LogTemp, Warning, TEXT("IS ALIVE CHECK"));
	return isAlive;
}

void AMultiFPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMultiFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMultiFPSCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AMultiFPSCharacter::GetHasRifle()
{
	return bHasRifle;
}

USkeletalMeshComponent* AMultiFPSCharacter::GetBodyComponent()
{
	return BodyMesh;
}

void AMultiFPSCharacter::Die()
{
	isAlive = false;
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// Отключаем ввод от игрока
		PC->DisableInput(PC);

		// Если вы хотите переключить камеру на другой вид
		// AMySpectatorCamera* SpectatorCamera = // Получаем или создаем спектаторскую камеру
		// PC->SetViewTargetWithBlend(SpectatorCamera, 1.0f, VTBlend_Linear);
        
		// Дополнительно, если требуется, можно скрыть интерфейс
		// PC->bShowMouseCursor = false;
		// PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);
	}
	
	AMultiFPSGameMode* GameMode = GetWorld()->GetAuthGameMode<AMultiFPSGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->OnPlayerDeath(this);
	}
}


