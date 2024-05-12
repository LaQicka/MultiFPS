// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretActor.h"

#include "MultiFPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurretActor::ATurretActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DAMAGE COMPONENT"));
	
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH HEAD"));
	HeadMesh->SetupAttachment(RootComponent);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH BASE"));
	BaseMesh->SetupAttachment(RootComponent);

	LookSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SightSphere"));
	LookSphere->SetupAttachment(RootComponent);
	LookSphere->InitSphereRadius(750.0f); // Установка радиуса обзора

	ForwardRef = CreateDefaultSubobject<USceneComponent>(TEXT("FORWARD REF"));
	ForwardRef->SetupAttachment(HeadMesh);
	
}

// Called when the game starts or when spawned
void ATurretActor::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Turret begin %s"), *HeadMesh->GetComponentLocation().ToString());
	Super::BeginPlay();
}

// Called every frame
void ATurretActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(TrackingPlayer && (!LookSphere->IsOverlappingActor(TrackingPlayer)
		|| !TrackingPlayer->isAlive || !HasDirectLineOfSightToActor(TrackingPlayer)))
	{
		TrackingPlayer = nullptr;
	}
	
	GetNearestPlayerPawn();
	
	if (TrackingPlayer && LookSphere->IsOverlappingActor(TrackingPlayer))
	{
		
		FVector DirectionToPlayer = TrackingPlayer->GetActorLocation() - HeadMesh->GetComponentLocation();
		FRotator TargetRotation = DirectionToPlayer.Rotation();

		// Коррекция с учетом начальной ориентации турели, если необходимо
		FRotator CurrentRotation = HeadMesh->GetComponentRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TrackingSpeed);

		HeadMesh->SetWorldRotation(NewRotation);

		if (FMath::Abs(NewRotation.Yaw - TargetRotation.Yaw) < 10 && 
			FMath::Abs(NewRotation.Pitch - TargetRotation.Pitch) < 10)
		{
			// Игрок в зоне, рисуем лазер
			FVector StartLocation = ForwardRef->GetComponentLocation();
			FVector EndLocation = TrackingPlayer->GetActorLocation();

			FColor LaserColor = FColor::Blue;


			AController* InstigatedBy = GetInstigatorController();
			TSubclassOf<UDamageType> DamageTypeClass;
			
			TrackingPlayer->RequestDrawLazerLine(StartLocation, EndLocation, LaserColor, TrackingPlayer, BaseDamage, this, InstigatedBy, DamageTypeClass);
		}
	}
}

void ATurretActor::GetNearestPlayerPawn()
{
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMultiFPSCharacter::StaticClass(), PlayerActors);

	AMultiFPSCharacter* ClosestPlayer = nullptr;
	float ClosestDistanceSqr = FLT_MAX;

	// Находим ближайшего игрока
	for (AActor* PlayerActor : PlayerActors)
	{
		AMultiFPSCharacter* PlayerCharacter = Cast<AMultiFPSCharacter>(PlayerActor);
		if (PlayerCharacter && LookSphere->IsOverlappingActor(PlayerCharacter) && PlayerCharacter->isAlive && HasDirectLineOfSightToActor(PlayerCharacter))
		{
			UE_LOG(LogTemp, Warning, TEXT("Turret tick %s"), *HeadMesh->GetComponentLocation().ToString());
			float DistanceSqr = (PlayerCharacter->GetActorLocation() - GetActorLocation()).SizeSquared();
			if (DistanceSqr < ClosestDistanceSqr)
			{
				ClosestDistanceSqr = DistanceSqr;
				ClosestPlayer = PlayerCharacter;
			}
		}
	}

	// Устанавливаем игрока как цель
	if(!TrackingPlayer && ClosestPlayer)
	{
		TrackingPlayer = ClosestPlayer;
		UE_LOG(LogTemp, Warning, TEXT("TARGET CHANGED"));
	}
}

bool ATurretActor::HasDirectLineOfSightToActor(AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return false;
	}

	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = TargetActor->GetActorLocation();
	FCollisionQueryParams CollisionParams;

	// Исключаем сам турель и целевой актор из проверки видимости
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(TargetActor);

	// Выполняем LineTrace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	// Проверяем, было ли столкновение и если да, то с целевым актором
	return !bHit || (bHit && HitResult.GetActor() == TargetActor);
}

float ATurretActor::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("D %f"), CurrentHealth);
	if(CurrentHealth <= 0)
	{
		Destroy();
	}
	// return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}



