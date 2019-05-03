// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GamePlayGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(BossDeadSignature);
DECLARE_MULTICAST_DELEGATE(PlayerDeadSignature);
DECLARE_DELEGATE_OneParam(StartSpawnNotesSignature, int32);

/**
 * ���� ������ �ִϸ��̼��� �����ϱ� ���� ����ϴ� ���°�
 * TODO: ���� ���͸� C++ �ڵ�� �� �� �ڼ��ϰ� �����ϰ� �ȴٸ� �� ������ �Űܾ� �Ѵ�.
 */
UENUM(BlueprintType)
enum class EBossActionState : uint8
{
	EIdle,
	EAttack,
	EDamaged,
	EDead,

	ENone,
};

/**
 * �� ���� �÷��� �� ���� �¸�, �й踦 �Ǵ��ϱ� ���� �����͸� �ٷ��.
 */
UCLASS()
class SWORDMASTER_API AGamePlayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	// ���� ���� ü���� ������� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentBossHealthRatio();

	// ���� �÷��̾� ü���� ������� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentPlayerHealthRatio();

	// ���� ������ ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const int32& GetCurrentScore();

	// ���� ������ ���¸� ��ȯ (���� ������ �ִϸ��̼ǿ��� ���ȴ�.)
	UFUNCTION(BlueprintCallable, Category = "Boss Action")
	EBossActionState GetCurrentBossActionState();

public:
	// ��� ��Ʈ�� ���� ���� �� ȣ��Ǵ� �ݹ�
	void OnAllNotesCleared();

	// ������ �����ϴ� ������ �߻����� �� ȣ��Ǵ� �ݹ�
	void OnAttackToBoss(int32 Damage);

	// �÷��̾ ���ظ� �޴� ������ �߻����� �� ȣ��Ǵ� �ݹ�
	void OnDamageFromBoss(int32 Damage);

public:
	// �������� �����͸� �ʱ�ȭ
	void InitializeLevel();

	// ���������͵��� �� �Լ����� �ϰ��� ���ε�
	void BindDelegateAll();

private:
	// ���� ���� ü��
	int32 CurrentBossHealth;

	// �ִ� ���� ü��
	int32 MaxBossHealth;

	// ���� �÷��̾� ü��
	int32 CurrentPlayerHealth;

	// �ִ� �÷��̾� ü��
	int32 MaxPlayerHealth;

	// ���� ����
	int32 CurrentScore;

	// ���� ������ �ൿ ����
	EBossActionState CurrentBossActionState;

private:
	// ������ �׾��� �� �˷��� ��������Ʈ
	BossDeadSignature BossDeadDeleate;

	// �÷��̾ �׾��� �� �˷��� ��������Ʈ
	PlayerDeadSignature PlayerDeadDelegate;

	// ���� �������� ��Ʈ���� ������Ű�� �����ص� ���� �˷��� ��������Ʈ
	StartSpawnNotesSignature StartSpawnNotesDelegate;
};
