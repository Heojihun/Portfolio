// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LevelDataManager.h"
#include "SMGameInstance.generated.h"

/** Enum to store the current action state */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	EIntro,
	EMainMenu,
	EGamePlay,
	EInterStage,

	ENone,
};

/**
 * 전체적인 게임 플로우를 관리
 * 전역적으로 사용될 데이터들을 관리
 */
UCLASS()
class SWORDMASTER_API USMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USMGameInstance();

	virtual void Init() override;

public:
	// 다음 게임 플로우로 전환
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void NextGameState();

	// 특정 게임 플로우로 전환
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void JumpToGameState(EGameState State);

	// 현재 게임 점수를 반환
	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetCurrentScore();

public:
	void OnIncreaseScore(int32 AddedScore);

public:
	// 현재 게임 플로우가 전환 됐을 때, 그 상태에 따른 처리
	void ProcessCurrentGameState();
	
	// 새로운 레벨을 로드한다. (OpenLevel)
	void LoadLevel(FString LevelName);

	const FLevelDataManager& GetLevelDataManager() { return LevelDataManager; }
	const FLevelData* GetCurrentLevelData() { return LevelDataManager.GetLevelData(CurrentLevelIndex); }

	int32 GetCurrentPlayerHealth() { return CurrentPlayerHealth; }
	void SetCurrentPlayerHealth(int32 Health) { CurrentPlayerHealth = Health; }

	float GetCurrentSwordSpeed() { return CurrentSwordSpeed; }
	void UpgradeSwordSpeed();

	float GetCurrentSwordRange() { return CurrentSwordRange; }
	void UpgradeSwordRange();

	int32 GetCurrentSwordMinPower() { return CurrentSwordMinPower; }
	int32 GetCurrentSwordMaxPower() { return CurrentSwordMaxPower; }

	int32 GetIncreasePowerWhenGood() { return IncreasePowerWhenGood; }
	int32 GetIncreasePowerWhenGreat() { return IncreasePowerWhenGreat; }
	int32 GetIncreasePowerWhenExcelent() { return IncreasePowerWhenExcelent; }

protected:
	/*
	 * 이 곳의 데이터들은 모두 GameInstance 블루프린트를 통해서 입력될 초기값이다.
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString IntroLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString MainMenuLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString InterStageLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 InitialPlayerHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float InitialSwordSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float InitialSwordRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 InitialSwordMinPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 InitialSwordMaxPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float IncreasedSwordSpeedPerGrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 MaxSwordSpeedGrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float IncreasedSwordRangePerGrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 MaxSwordRangeGrade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 IncreasePowerWhenGood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 IncreasePowerWhenGreat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 IncreasePowerWhenExcelent;

private:
	// 현재 게임 플로우 상태
	EGameState GameState;

	// 레벨 데이터
	FLevelDataManager LevelDataManager;

	// 현재 레벨
	int32 CurrentLevelIndex;

	// 현재 플레이어의 상태값들
	int32 CurrentPlayerHealth;
	float CurrentSwordSpeed;
	float CurrentSwordRange;
	int32 CurrentSwordMinPower;
	int32 CurrentSwordMaxPower;
	int32 CurrentSwordSpeedGrade;
	int32 CurrentSwordRangeGrade;

	// 현재 점수
	int32 CurrentScore;
};
