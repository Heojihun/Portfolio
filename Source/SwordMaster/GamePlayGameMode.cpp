// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "GamePlayGameMode.h"
#include "SMGameInstance.h"

AGamePlayGameMode::AGamePlayGameMode()
{
}

void AGamePlayGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AGamePlayGameMode::OnBossDead()
{
	ChangeMenuWidget(GameWinWidgetClass);
}

void AGamePlayGameMode::OnPlayerDead()
{
	ChangeMenuWidget(GameLoseWidgetClass);
}
