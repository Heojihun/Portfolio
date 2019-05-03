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
 * LevelData.xml�� �Ľ��ؼ� �����͸� �����Ѵ�.
 */
class SWORDMASTER_API FLevelDataManager
{
public:
	~FLevelDataManager();

public:
	// LevelData.xml�� �д´�.
	void ReadLevelDatas();

	// ���� ���� ���� �����͸� ��� �����Ѵ�.
	void ClearAllLevelDatas();

	// �ִ� ���� ������ Ȯ��
	int32 GetLevelCount();

	// Ư�� ������ �����͸� ��´�.
	const FLevelData* GetLevelData(int32 Index);

	// ������ �������� Ȯ��
	bool IsLastLevel(int32 Index);

private:
	// LevelData.xml�κ��� ���� �����͵��� ����
	TArray<FLevelData*> LevelDatas;
};
