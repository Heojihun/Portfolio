// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class ENoteType
{
	E_AttackNote,
	E_GuardNote,
	E_SkillNote,
	E_None
};

struct FNotePattern
{
	ENoteType Type;
	float SpawnTime;
};

struct FNotePhase
{
	float BossHealthRate;
	float NoteSpeed;

	TArray<FNotePattern> NotePatterns;
};

struct FLevelData
{
	FString LevelName;
	int32 BossIndex;
	int32 BossHealth;
	int32 BossAttackPower;

	TArray<FNotePhase*> NotePhases;
};

/**
 * LevelData.xml을 파싱해서 데이터를 보관한다.
 */
class SWORDMASTER_API FLevelDataManager
{
public:
	~FLevelDataManager();

public:
	// LevelData.xml을 읽는다.
	void ReadLevelDatas();

	// 현재 보관 중인 데이터를 모두 정리한다.
	void ClearAllLevelDatas();

	// 최대 레벨 개수를 확인
	int32 GetLevelCount();

	// 특정 레벨의 데이터를 얻는다.
	const FLevelData* GetLevelData(int32 Index);

	// 마지막 레벨인지 확인
	bool IsLastLevel(int32 Index);

private:
	// LevelData.xml로부터 읽은 데이터들을 보관
	TArray<FLevelData*> LevelDatas;
};
