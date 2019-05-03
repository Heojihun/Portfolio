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
 * ��ü���� ���� �÷ο츦 ����
 * ���������� ���� �����͵��� ����
 */
UCLASS()
class SWORDMASTER_API USMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USMGameInstance();

	virtual void Init() override;

public:
	// ���� ���� �÷ο�� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void NextGameState();

	// Ư�� ���� �÷ο�� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void JumpToGameState(EGameState State);

	// ���� ���� ������ ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetCurrentScore();

public:
	void OnIncreaseScore(int32 AddedScore);

public:
	// ���� ���� �÷ο찡 ��ȯ ���� ��, �� ���¿� ���� ó��
	void ProcessCurrentGameState();
	
	// ���ο� ������ �ε��Ѵ�. (OpenLevel)
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
	 * �� ���� �����͵��� ��� GameInstance �������Ʈ�� ���ؼ� �Էµ� �ʱⰪ�̴�.
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
	// ���� ���� �÷ο� ����
	EGameState GameState;

	// ���� ������
	FLevelDataManager LevelDataManager;

	// ���� ����
	int32 CurrentLevelIndex;

	// ���� �÷��̾��� ���°���
	int32 CurrentPlayerHealth;
	float CurrentSwordSpeed;
	float CurrentSwordRange;
	int32 CurrentSwordMinPower;
	int32 CurrentSwordMaxPower;
	int32 CurrentSwordSpeedGrade;
	int32 CurrentSwordRangeGrade;

	// ���� ����
	int32 CurrentScore;
};
