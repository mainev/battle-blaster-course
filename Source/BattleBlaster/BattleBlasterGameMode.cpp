// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Tower.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);

	TowerCount = Towers.Num();


	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn) {
		Tank = Cast<ATank>(PlayerPawn);

		if (Tank == nullptr) {
			UE_LOG(LogTemp, Display, TEXT("GameMode: Failed to find the tank actor."));
		}
		else {
			int32 LoopIndex = 0;
			while (LoopIndex < TowerCount) {

				AActor* TowerActor = Towers[LoopIndex];

				if (TowerActor) {
					ATower* Tower = Cast<ATower>(TowerActor);

					if (Tower) {
						Tower->Tank = Tank;

					}
				}

				LoopIndex++;
			}
		}
	}





}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Tank) {
		Tank->HandleDestruction();
	}
	else {
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (DeadTower)
		{

			DeadTower->HandleDestruction();
			TowerCount--;

			if (TowerCount == 0) {
				// VICTORY!!
			}
		}
	}
}
