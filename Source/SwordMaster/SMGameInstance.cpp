// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "SMGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"

USMGameInstance::USMGameInstance()
	: GameState(EGameState::EIntro)
	, CurrentLevelIndex(0)
	, CurrentScore(0)
{
}

void USMGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("USMGameInstance::Init"));

	CurrentPlayerHealth = InitialPlayerHealth;
	CurrentSwordSpeed = InitialSwordSpeed;
	CurrentSwordRange = InitialSwordRange;
	CurrentSwordMinPower = InitialSwordMinPower;
	CurrentSwordMaxPower = InitialSwordMaxPower;

	LevelDataManager.ReadLevelDatas();
	ProcessCurrentGameState();
}

void USMGameInstance::NextGameState()
{
	// ���� �÷ο� ������ ���������� ������, InterStage ���Ŀ� �ٽ� GamePlay�� �ݺ��Ѵ�.

	if (GameState == EGameState::EInterStage)
	{
		GameState = EGameState::EGamePlay;
	}
	else
	{
		GameState = (EGameState)(((int32)GameState) + 1);
	}

	ProcessCurrentGameState();
}

void USMGameInstance::JumpToGameState(EGameState State)
{
	if (GameState != State)
	{
		GameState = State;
		ProcessCurrentGameState();
	}
}

int32 USMGameInstance::GetCurrentScore()
{
	return CurrentScore;
}

void USMGameInstance::OnIncreaseScore(int32 AddedScore)
{
	CurrentScore += AddedScore;
}

void USMGameInstance::ProcessCurrentGameState()
{
	switch (GameState)
	{
	case EGameState::EIntro:
		NextGameState();
		break;

	case EGameState::EMainMenu:
		{
			LoadLevel(MainMenuLevelName);
			CurrentLevelIndex = 0;
		}
		break;

	case EGameState::EGamePlay:
		{
			verify(false == LevelDataManager.IsLastLevel(CurrentLevelIndex));
			const FLevelData* Level = LevelDataManager.GetLevelData(CurrentLevelIndex);
			LoadLevel(Level->LevelName);
		}
		break;

	case EGameState::EInterStage:
		{
			LoadLevel(InterStageLevelName);
			CurrentLevelIndex++;
		}
		break;
	}
}

void USMGameInstance::LoadLevel(FString LevelName)
{
	FString CurrentLevel = GetWorld()->GetMapName();

	if (CurrentLevel != LevelName)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
	}
}

void USMGameInstance::UpgradeSwordSpeed()
{

}

void USMGameInstance::UpgradeSwordRange()
{

}