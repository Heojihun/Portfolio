// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "LevelDataManager.h"
#include "AssertionMacros.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/XmlParser/Public/XmlFile.h"

FLevelDataManager::~FLevelDataManager()
{
	ClearAllLevelDatas();
}

void FLevelDataManager::ReadLevelDatas()
{
	ClearAllLevelDatas();

	FString LevelDataFilePath = FPaths::GameContentDir();
	LevelDataFilePath.Append("\\Datas\\LevelData.xml");
	const FXmlFile DataFile(LevelDataFilePath);

	verify(DataFile.IsValid());

	const FXmlNode* DataNode = DataFile.GetRootNode();

	verify(DataNode);

	const TArray<FXmlNode*> LevelNodes = DataNode->GetChildrenNodes();
	for (FXmlNode* LevelNode : LevelNodes)
	{
		verify(LevelNode);

		FLevelData* NewLevelData = new FLevelData();
		NewLevelData->LevelName = LevelNode->GetAttribute("LevelName");
		NewLevelData->BossIndex = FCString::Atoi(*(LevelNode->GetAttribute("BossIndex")));
		NewLevelData->BossHealth = FCString::Atoi(*(LevelNode->GetAttribute("BossHealth")));
		NewLevelData->BossAttackPower = FCString::Atoi(*(LevelNode->GetAttribute("BossAttackPower")));

		const TArray<FXmlNode*> NotePhaseNodes = LevelNode->GetChildrenNodes();
		for (FXmlNode* NotePhaseNode : NotePhaseNodes)
		{
			verify(NotePhaseNode);

			FNotePhase* NewNotePhase = new FNotePhase();
			NewNotePhase->BossHealthRate = FCString::Atof(*(NotePhaseNode->GetAttribute("BossHealthRate")));
			NewNotePhase->NoteSpeed = FCString::Atof(*(NotePhaseNode->GetAttribute("NoteSpeed")));

			const TArray<FXmlNode*> NotePatternNodes = NotePhaseNode->GetChildrenNodes();
			for (FXmlNode* NotePatternNode : NotePatternNodes)
			{
				verify(NotePatternNode);

				FNotePattern NewNotePattern;
				NewNotePattern.Type = static_cast<ENoteType>(FCString::Atoi(*(NotePatternNode->GetAttribute("Type"))));
				NewNotePattern.SpawnTime = FCString::Atof(*(NotePatternNode->GetAttribute("SpawnTime")));

				NewNotePhase->NotePatterns.Add(NewNotePattern);
			}

			NewLevelData->NotePhases.Add(NewNotePhase);
		}

		LevelDatas.Add(NewLevelData);
	}
}

void FLevelDataManager::ClearAllLevelDatas()
{
	for (FLevelData* Data : LevelDatas)
	{
		for (FNotePhase* Phase : Data->NotePhases)
		{
			delete Phase;
		}

		delete Data;
	}
}

int32 FLevelDataManager::GetLevelCount()
{
	return LevelDatas.Num();
}

const FLevelData* FLevelDataManager::GetLevelData(int32 Index)
{
	verify(LevelDatas.IsValidIndex(Index));

	return LevelDatas[Index];
}

bool FLevelDataManager::IsLastLevel(int32 Index)
{
	return GetLevelCount() <= Index;
}
