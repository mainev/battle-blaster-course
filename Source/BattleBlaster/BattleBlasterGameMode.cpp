// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

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


	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");

		}

	}

	CountdownSeconds = CountdownDelay;

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.0f, true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	CountdownSeconds--;

	if (CountdownSeconds > 0) {
		//UE_LOG(LogTemp, Display, TEXT("Countdown: %d"), CountdownSeconds);
		ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownSeconds));
	}
	else if (CountdownSeconds == 0)
	{
		// Game started
		ScreenMessageWidget->SetMessageText("GO!");
		Tank->SetPlayerEnabled(true);
	}
	else
	{
		// CountdownSeconds is less than 0
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	
}


void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool IsGameOver = false;


	if (DeadActor == Tank) {
		Tank->HandleDestruction();

		IsVictory = false;
		IsGameOver = true;
	}
	else {
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (DeadTower)
		{

			DeadTower->HandleDestruction();
			TowerCount--;

			if (TowerCount == 0) {
				// VICTORY!!

				IsVictory = true;
				IsGameOver = true;
			}
		}
	}

	if (IsGameOver) {

		FString GameOverString = IsVictory ? "Victory" : "Defeat";

		//UE_LOG(LogTemp, Display, TEXT("Game Over: %s"), *GameOverString);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		ScreenMessageWidget->SetMessageText(FString::Printf(TEXT("Game Over: %s"), *GameOverString));

		FTimerHandle GameOverTimerHandle ;
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOverTimerTimeout, GameOverDelay, false);
	}
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{
	//UE_LOG(LogTemp, Display, TEXT("Game over timer timeut!"));

	// restart the level
	//FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);
		if (BattleBlasterGameInstance)
		{
			if (IsVictory) {
				// Load the next level
				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				// Reload the current level
				BattleBlasterGameInstance->RestartCurrentLevel();
			}
		}
	}
	

	
}

