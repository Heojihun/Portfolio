// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "GamePlayPlayerController.h"
#include "GamePlayGameState.h"
#include "SMGameInstance.h"
#include "Engine/World.h"

AGamePlayPlayerController::AGamePlayPlayerController()
	: bIsRunning(true)
	, SwordSpeed(0.f)
	, SwordRange(0.f)
	, SwordMinPower(0)
	, SwordMaxPower(0)
	, SwordAngle(0.f)
	, SwordPower(0)
	, BossAttackPower(1)
	, bIsActionKeyPressed(false)
	, bShouldCheckMiss(false)
	, GuardStartAngle(0.f)
	, CurrentActionState(EActionState::ENone)
	, RunningNoteSpawnTime(0.0f)
	, SpawnTargetNoteIndex(0)
	, bIsAllSpawnNotesInPhase(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGamePlayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameAndUI());

	InitializePlayerStat();

	BindDelegateAll();

	bIsRunning = true;
	CurrentActionState = EActionState::EEmpty;
}

void AGamePlayPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRunning)
	{
		UpdateSpawnNote(DeltaTime);

		MoveSword(DeltaTime);

		MoveNotes(DeltaTime);

		UpdateGuardNote();
	}
}

float AGamePlayPlayerController::GetSwordAngle()
{
	return SwordAngle;
}

bool AGamePlayPlayerController::IsAttackNoteAlive(int32 index)
{
	verify(index >= 0 && index < ATTACK_NOTE_COUNT);
	return AttackNotes[index].bAlive;
}

const FVector2D& AGamePlayPlayerController::GetAttackNoteDirection(int32 index)
{
	verify(index >= 0 && index < ATTACK_NOTE_COUNT);
	return AttackNotes[index].Direction;
}

bool AGamePlayPlayerController::IsGuardNoteAlive(int32 index)
{
	verify(index >= 0 && index < GUARD_NOTE_COUNT);
	return GuardNotes[index].bAlive;
}

const FVector2D& AGamePlayPlayerController::GetGuardNoteDirection(int32 index)
{
	verify(index >= 0 && index < GUARD_NOTE_COUNT);
	return GuardNotes[index].Direction;
}

bool AGamePlayPlayerController::IsSkillNoteAlive(int32 index)
{
	verify(index >= 0 && index < SKILL_NOTE_COUNT);
	return SkillNotes[index].bAlive;
}

const FVector2D& AGamePlayPlayerController::GetSkillNoteDirection(int32 index)
{
	verify(index >= 0 && index < SKILL_NOTE_COUNT);
	return SkillNotes[index].Direction;
}

EActionState AGamePlayPlayerController::GetCurrentActionState()
{
	EActionState ActionState = CurrentActionState;
	CurrentActionState = EActionState::EEmpty;
	return ActionState;
}

float AGamePlayPlayerController::GetCurrentSwordPowerRatio()
{
	float Ratio = (float)(SwordPower - SwordMinPower) / (float)(SwordMaxPower - SwordMinPower);
	return Ratio;
}

void AGamePlayPlayerController::OnBossDead()
{
	bIsRunning = false;
}

void AGamePlayPlayerController::OnPlayerDead()
{
	bIsRunning = false;
}

void AGamePlayPlayerController::OnStartSpawnNotes(int32 BossHealth)
{
	USMGameInstance* GameInstance = Cast<USMGameInstance>(GetGameInstance());
	verify(GameInstance != nullptr);

	const FLevelData* LevelData = GameInstance->GetCurrentLevelData();
	verify(LevelData != nullptr);

	BossAttackPower = LevelData->BossAttackPower;

	// ������ ü���� �������� ���� �����Ϳ��� � �������� ��Ʈ���� ������� ����
	float CurrentBossHealthRate = (float)BossHealth / (float)(LevelData->BossHealth);
	for (int i = LevelData->NotePhases.Num() - 1; i >= 0; --i)
	{
		if (LevelData->NotePhases[i]->BossHealthRate >= CurrentBossHealthRate)
		{
			NotePhaseData = LevelData->NotePhases[i];
			break;
		}
	}
	verify(NotePhaseData != nullptr);

	RunningNoteSpawnTime = 0.f;
	SpawnTargetNoteIndex = 0;

	bIsAllSpawnNotesInPhase = false;
}

void AGamePlayPlayerController::OnActionKeyPressed()
{
	if (!bIsRunning)
	{
		return;
	}

	bIsActionKeyPressed = true;
	GuardStartAngle = SwordAngle;

	ActionToAttackNote();
}

void AGamePlayPlayerController::OnActionKeyReleased()
{
	if (!bIsRunning)
	{
		return;
	}

	bIsActionKeyPressed = false;

	if (bShouldCheckMiss)
	{
		ChangeActionState(EActionState::EMiss);
	}
}

void AGamePlayPlayerController::InitializePlayerStat()
{
	USMGameInstance* GameInstance = Cast<USMGameInstance>(GetGameInstance());
	verify(GameInstance != nullptr);

	SwordSpeed = GameInstance->GetCurrentSwordSpeed();
	SwordRange = GameInstance->GetCurrentSwordRange();
	SwordMinPower = GameInstance->GetCurrentSwordMinPower();
	SwordMaxPower = GameInstance->GetCurrentSwordMaxPower();

	SwordAngle = 0.0f;
	SwordPower = SwordMinPower;
}

void AGamePlayPlayerController::BindDelegateAll()
{
	InputComponent->BindAction("Action", IE_Pressed, this, &AGamePlayPlayerController::OnActionKeyPressed);
	InputComponent->BindAction("Action", IE_Released, this, &AGamePlayPlayerController::OnActionKeyReleased);

	AGamePlayGameState* GameState = dynamic_cast<AGamePlayGameState*>(GetWorld()->GetGameState());
	verify(GameState != nullptr);
	AllNotesClearedDelegate.BindUObject(GameState, &AGamePlayGameState::OnAllNotesCleared);
	AttackToBossDelegate.BindUObject(GameState, &AGamePlayGameState::OnAttackToBoss);
	DamageFromBossDelegate.BindUObject(GameState, &AGamePlayGameState::OnDamageFromBoss);

	USMGameInstance* GameInstance = Cast<USMGameInstance>(GetGameInstance());
	verify(GameInstance != nullptr);
	IncreaseScoreDelegate.BindUObject(GameInstance, &USMGameInstance::OnIncreaseScore);
}

void AGamePlayPlayerController::UpdateSpawnNote(float DeltaTime)
{
	if (bIsAllSpawnNotesInPhase || !NotePhaseData)
	{
		return;
	}

	if (NotePhaseData->NotePatterns.Num() <= SpawnTargetNoteIndex)
	{
		return;
	}

	// ������ ������ �ð����κ��� ������ �ð��� Ȱ���Ͽ� ���� ��Ʈ�� �������Ѿ� �� Ÿ�̹��̶�� ���ο� ��Ʈ�� �����Ѵ�.
	// �� ��Ʈ �迭���� bAlive�� ���� �ִ� ��Ʈ�� Ȱ���Ѵ�.
	RunningNoteSpawnTime += DeltaTime;
	float TargetNoteSpawnTime = NotePhaseData->NotePatterns[SpawnTargetNoteIndex].SpawnTime;
	if (TargetNoteSpawnTime <= RunningNoteSpawnTime)
	{
		if (NotePhaseData->NotePatterns.Num() - 1 == SpawnTargetNoteIndex)
		{
			bIsAllSpawnNotesInPhase = true;
		}

		switch (NotePhaseData->NotePatterns[SpawnTargetNoteIndex].Type)
		{
			case ENoteType::E_AttackNote:
			{
				for (int i = 0; i < ATTACK_NOTE_COUNT; ++i)
				{
					if (!AttackNotes[i].bAlive)
					{
						AttackNotes[i].bAlive = true;
						AttackNotes[i].Angle = 0;
						break;
					}
				}
			} break;

			case ENoteType::E_GuardNote:
			{
				for (int i = 0; i < GUARD_NOTE_COUNT; ++i)
				{
					if (!GuardNotes[i].bAlive)
					{
						GuardNotes[i].bAlive = true;
						GuardNotes[i].Angle = 0;
						break;
					}
				}
			} break;
		}

		SpawnTargetNoteIndex++;
	}
}

void AGamePlayPlayerController::MoveSword(float DeltaTime)
{
	SwordAngle -= (DeltaTime * SwordSpeed);
	if (SwordAngle < -360.f)
	{
		SwordAngle += 360.f;

		// Į�� �ѹ��� ȸ�� ���� ��, ��� ��Ʈ�� ���� �ƴ��� Ȯ���ؼ� �̺�Ʈ�� �߻���Ų��.
		if (IsAllSpawnNotesHide())
		{
			AllNotesClearedDelegate.Execute();
		}
	}
}

void AGamePlayPlayerController::MoveNotes(float DeltaTime)
{
	if (NotePhaseData == nullptr)
	{
		return;
	}

	float MoveAngle = DeltaTime * NotePhaseData->NoteSpeed;

	for (int i = 0; i < ATTACK_NOTE_COUNT; ++i)
	{
		MoveNote(MoveAngle, AttackNotes[i]);
	}

	for (int i = 0; i < GUARD_NOTE_COUNT; ++i)
	{
		MoveNote(MoveAngle, GuardNotes[i]);
	}
}

void AGamePlayPlayerController::MoveNote(float MoveAngle, FNote& Note)
{
	if (!Note.bAlive)
	{
		return;
	}

	Note.Angle += MoveAngle;
	if (Note.Angle > 360.f)
	{
		// �� ���� ȸ���� ��Ʈ�� Miss ó���ϰ� �����Ѵ�.
		Note.bAlive = false;
		ChangeActionState(EActionState::EMiss);
	}
	else
	{
		// UI�� ���� ��Ʈ�� ȸ�� ������ ���صд�. (�ݽð� ������ ȸ���� �� �ְ� �Ѵ�.)
		float Theta = FMath::DegreesToRadians(Note.Angle - 90);
		FMath::SinCos(&Note.Direction.Y, &Note.Direction.X, Theta);
	}
}

void AGamePlayPlayerController::ActionToAttackNote()
{
	bShouldCheckMiss = true;
	for (int i = 0; i < ATTACK_NOTE_COUNT; ++i)
	{
		if (AttackNotes[i].bAlive)
		{
			float TargetSwordAngle = 360.f + SwordAngle;
			float TargetAngle = AttackNotes[i].Angle;
			float DiffAngle = FMath::Abs(TargetSwordAngle - TargetAngle);

			// Į�� �ִ� ������ 30% �ȿ� ��� �Դٸ� Excelent
			if (DiffAngle < (SwordRange * 0.3f))
			{
				ChangeActionState(EActionState::EExcelent);
				AttackNotes[i].bAlive = false;
				bShouldCheckMiss = false;
				break;
			}
			// Į�� �ִ� ������ 50% �ȿ� ��� �Դٸ� Great
			else if (DiffAngle < (SwordRange * 0.5f))
			{
				ChangeActionState(EActionState::EGreat);
				AttackNotes[i].bAlive = false;
				bShouldCheckMiss = false;
				break;
			}
			// Į�� �ִ� ���� �ȿ� ��� �Դٸ� Good
			else if (DiffAngle < SwordRange)
			{
				ChangeActionState(EActionState::EGood);
				AttackNotes[i].bAlive = false;
				bShouldCheckMiss = false;
				break; 
			}
		}
	}
}

void AGamePlayPlayerController::UpdateGuardNote()
{
	for (int i = 0; i < GUARD_NOTE_COUNT; ++i)
	{
		if (GuardNotes[i].bAlive)
		{
			float TargetSwordAngle = 360.f + SwordAngle;
			float TargetAngle = GuardNotes[i].Angle;

			if (bIsActionKeyPressed)
			{
				// �� �����ߴٸ� ��� ��Ʈ�� ����
				float StartSwordAngle = 360.f + GuardStartAngle;
				if (StartSwordAngle > TargetAngle && TargetSwordAngle <= TargetAngle)
				{
					ChangeActionState(EActionState::EGuard);
					GuardNotes[i].bAlive = false;
					bShouldCheckMiss = false;
				}
			}
			else
			{
				// ��� ��Ʈ�� ������ ��, ����ϰ� ���� �ʾҴٸ� �÷��̾�� ������ ó��
				float DiffAngle = FMath::Abs(TargetSwordAngle - TargetAngle);
				if (DiffAngle < 3.f)
				{
					ChangeActionState(EActionState::EDamaged);
					GuardNotes[i].bAlive = false;
				}
			}
		}
	}
}

bool AGamePlayPlayerController::IsAllSpawnNotesHide()
{
	if (bIsAllSpawnNotesInPhase)
	{
		for (int i = 0; i < ATTACK_NOTE_COUNT; ++i)
		{
			if (AttackNotes[i].bAlive)
			{
				return false;
			}
		}

		for (int i = 0; i < GUARD_NOTE_COUNT; ++i)
		{
			if (GuardNotes[i].bAlive)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void AGamePlayPlayerController::ChangeActionState(EActionState ActionState)
{
	USMGameInstance* GameInstance = Cast<USMGameInstance>(GetGameInstance());
	verify(GameInstance != nullptr);

	CurrentActionState = ActionState;

	switch (CurrentActionState)
	{
	case EActionState::EGood:
		IncreaseSwordPower(GameInstance->GetIncreasePowerWhenGood());
		AttackToBossDelegate.Execute(SwordPower);
		IncreaseScoreDelegate.Execute(SwordPower * 10);
		break;

	case EActionState::EGreat:
		IncreaseSwordPower(GameInstance->GetIncreasePowerWhenGreat());
		AttackToBossDelegate.Execute(SwordPower);
		IncreaseScoreDelegate.Execute(SwordPower * 10);
		break;

	case EActionState::EExcelent:
		IncreaseSwordPower(GameInstance->GetIncreasePowerWhenExcelent());
		AttackToBossDelegate.Execute(SwordPower);
		IncreaseScoreDelegate.Execute(SwordPower * 10);
		break;

	case EActionState::EMiss:
		SwordPower = SwordMinPower;
		break;

	case EActionState::EGuard:
		IncreaseScoreDelegate.Execute(100);
		break;

	case EActionState::EDamaged:
		DamageFromBossDelegate.Execute(BossAttackPower);
		break;
	}
}

void AGamePlayPlayerController::IncreaseSwordPower(int32 IncreasePower)
{
	SwordPower = FMath::Min(SwordMaxPower, SwordPower + IncreasePower);
}