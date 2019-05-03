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

	// 보스의 체력을 기준으로 레벨 데이터에서 어떤 페이즈의 노트들을 사용할지 결정
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

	// 스폰을 시작한 시간으로부터 누적된 시간을 활용하여 현재 노트를 스폰시켜야 될 타이밍이라면 새로운 노트를 스폰한다.
	// 각 노트 배열에서 bAlive가 꺼져 있는 노트를 활용한다.
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

		// 칼이 한바퀴 회전 했을 때, 모든 노트가 정리 됐는지 확인해서 이벤트를 발생시킨다.
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
		// 한 바퀴 회전된 노트는 Miss 처리하고 제거한다.
		Note.bAlive = false;
		ChangeActionState(EActionState::EMiss);
	}
	else
	{
		// UI를 위해 노트의 회전 방향을 구해둔다. (반시계 방향을 회전될 수 있게 한다.)
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

			// 칼의 최대 범위의 30% 안에 들어 왔다면 Excelent
			if (DiffAngle < (SwordRange * 0.3f))
			{
				ChangeActionState(EActionState::EExcelent);
				AttackNotes[i].bAlive = false;
				bShouldCheckMiss = false;
				break;
			}
			// 칼의 최대 범위의 50% 안에 들어 왔다면 Great
			else if (DiffAngle < (SwordRange * 0.5f))
			{
				ChangeActionState(EActionState::EGreat);
				AttackNotes[i].bAlive = false;
				bShouldCheckMiss = false;
				break;
			}
			// 칼의 최대 범위 안에 들어 왔다면 Good
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
				// 방어에 성공했다면 방어 노트를 제거
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
				// 방어 노트가 지나갈 때, 방어하고 있지 않았다면 플레이어에게 데미지 처리
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