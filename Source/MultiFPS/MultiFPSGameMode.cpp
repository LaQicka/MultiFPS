// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiFPSGameMode.h"
#include "MultiFPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMultiFPSGameMode::AMultiFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AMultiFPSGameMode::OnPlayerDeath(ACharacter* DeadPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Another dead today"));
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		AMultiFPSCharacter* Character = Cast<AMultiFPSCharacter>(PlayerController->GetCharacter());
		if (PlayerController && Character->isAlive)
		{
			// Если есть хоть один живой игрок, выходим
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("All dead"));
	
	// Если ни одного живого игрока не найдено, рестартуем уровень
	RestartLevel();
}

void AMultiFPSGameMode::RestartLevel()
{
	// Получаем имя текущего уровня
	FString CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this, true);

	// Перезапускаем уровень, используя ServerTravel
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(CurrentLevelName);
	}
}
