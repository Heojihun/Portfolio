// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Core/Public/Containers/Queue.h"
#include "SMGameInstance.h"
#include "GamePlayPlayerController.generated.h"

#define ATTACK_NOTE_COUNT 10
#define GUARD_NOTE_COUNT 10
#define SKILL_NOTE_COUNT 3

DECLARE_DELEGATE(AllNotesClearedSignature);
DECLARE_DELEGATE_OneParam(AttackToBossSignature, int32);
DECLARE_DELEGATE_OneParam(DamageFromBossSignature, int32);
DECLARE_DELEGATE_OneParam(IncreaseScoreSignature, int32);

/** Enum to store the current action state */
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EEmpty,
	EGood,
	EGreat,
	EExcelent,
	EMiss,
	EGuard,
	EDamaged,

	ENone,
};

/**
 * �������� ���� ������ ����Ѵ�.
 * ��Ʈ�� ������Ʈ �� ��Ʈ ������ �Ѵ�.
 */
UCLASS()
class SWORDMASTER_API AGamePlayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGamePlayPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// ��Ʈ�� ���¸� ��Ÿ���� ����ü
	struct FNote
	{
		// ��Ʈ�� ȭ�鿡 �������� ������Ʈ ����?
		bool bAlive;
		// ��Ʈ�� ȸ���� ��Ʈ ������ �ϱ� ���� ��
		float Angle;
		// UI���� ȭ�鿡 ��Ʈ�� �����ֱ� ���� ����ϴ� ��
		FVector2D Direction;
	};

public:
	// UI���� Į�� ȸ���� ǥ���ϱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetSwordAngle();

	// UI���� ȭ�鿡 ���� ��Ʈ�� �������� �Ǵ�
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsAttackNoteAlive(int32 index);

	// UI���� ȭ�鿡 ���� ��Ʈ�� ��ġ�� �����ֱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetAttackNoteDirection(int32 index);

	// UI���� ȭ�鿡 ��� ��Ʈ�� �������� �Ǵ�
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsGuardNoteAlive(int32 index);

	// UI���� ȭ�鿡 ��� ��Ʈ�� ��ġ�� �����ֱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetGuardNoteDirection(int32 index);

	// UI���� ȭ�鿡 ��ų ��Ʈ�� �������� �Ǵ�
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsSkillNoteAlive(int32 index);

	// UI���� ȭ�鿡 ��ų ��Ʈ�� ��ġ�� �����ֱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetSkillNoteDirection(int32 index);

	// UI�� ���� ���¸� ǥ���ϱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	EActionState GetCurrentActionState();

	// UI�� ���� ������ �Ŀ��� ǥ���ϱ� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentSwordPowerRatio();

public:
	// ������ �׾��� �� ȣ��Ǵ� �ݹ�
	void OnBossDead();

	// �÷��̾ �׾��� �� ȣ��Ǵ� �ݹ�
	void OnPlayerDead();

	// ��Ʈ�� ������ ���� ���Ѿ� �� �� ȣ��Ǵ� �ݹ�
	void OnStartSpawnNotes(int32 BossHealth);

private:
	// Ű �Է�
	void OnActionKeyPressed();
	void OnActionKeyReleased();

private:
	// �÷��̾��� ������ �ʱ�ȭ
	void InitializePlayerStat();

	// ���������͵��� �� �Լ����� �ϰ��� ���ε�
	void BindDelegateAll();

	// ���� ������ų ��Ʈ�� �ִ��� Ȯ���Ͽ� ó��
	void UpdateSpawnNote(float DeltaTime);
	
	// Į�� ȸ��
	void MoveSword(float DeltaTime);

	// ��Ʈ���� ȸ��
	void MoveNotes(float DeltaTime);
	void MoveNote(float MoveAngle, FNote& Note);

	// �׼�Ű�� �Է� �Ǿ��� ��, ��Ʈ�� ��Ʈ�� �ִ��� Ȯ���Ͽ� ó��
	void ActionToAttackNote();

	// ��� ��Ʈ���� ��ȣ�ۿ� ó��
	void UpdateGuardNote();

	// �̹� �������� ��� ��Ʈ�� ���� �ƴ��� Ȯ��
	bool IsAllSpawnNotesHide();

	// �ൿ ���¸� �����ϰ� �׿� ���� ó��
	void ChangeActionState(EActionState ActionState);

	// �÷��̾��� ���ݷ��� ����
	void IncreaseSwordPower(int32 IncreasePower);

private:
	// ���� ������ ������Ʈ ���� ����
	bool bIsRunning;

	// Į�� ȸ�� �ӵ�, �ǰ� ����, �ּ�/�ִ� �Ŀ�
	float SwordSpeed;
	float SwordRange;
	int32 SwordMinPower;
	int32 SwordMaxPower;

	// ���� Į�� ȸ����
	float SwordAngle;

	// ���� Į�� ���ݷ�
	int32 SwordPower;

	// ������ ���ݷ�
	int32 BossAttackPower;

	// ����/���/��ų ��Ʈ��
	FNote AttackNotes[ATTACK_NOTE_COUNT];
	FNote GuardNotes[GUARD_NOTE_COUNT];
	FNote SkillNotes[SKILL_NOTE_COUNT];

	// �׼�Ű�� ���� �ִ� �������� Ȯ��
	bool bIsActionKeyPressed;

	// ���� �׼��� ���з� ó���ؾ� ����?
	bool bShouldCheckMiss;

	// �� ������ ȸ����
	float GuardStartAngle;

	// ���� �׼� ����
	EActionState CurrentActionState;

	// ���� ����� �ش��ϴ� ��Ʈ ������
	const FNotePhase* NotePhaseData;

	// ��Ʈ�� �����ϱ� �������� �� ���� �����Ǿ����� ��.
	// �� ���� �������� ���� ��Ʈ�� ������ Ÿ�̹��� ��������.
	float RunningNoteSpawnTime;

	// ���� �����Ǿ��� ��Ʈ�� �ε���
	int32 SpawnTargetNoteIndex;

	// ���� �������� ��� ��Ʈ�� �����ߴ��� Ȯ��
	bool bIsAllSpawnNotesInPhase;

private:
	// ��� ��Ʈ�� ���� ���� �� ȣ��Ǵ� ��������Ʈ
	AllNotesClearedSignature AllNotesClearedDelegate;

	// ������ �����ϴ� ������ ������ �� ȣ��Ǵ� ��������Ʈ
	AttackToBossSignature AttackToBossDelegate;

	// �÷��̾ ������ �޴� ������ ������ �� ȣ��Ǵ� ��������Ʈ
	DamageFromBossSignature DamageFromBossDelegate;

	// ������ �������Ѿ� �� �� ȣ��Ǵ� ��������Ʈ
	IncreaseScoreSignature IncreaseScoreDelegate;
};
