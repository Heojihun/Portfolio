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
 * 실질적인 게임 진행을 담당한다.
 * 노트의 업데이트 및 히트 판정을 한다.
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
	// 노트의 상태를 나타내는 구조체
	struct FNote
	{
		// 노트가 화면에 보여지고 업데이트 될지?
		bool bAlive;
		// 노트의 회전과 히트 판정을 하기 위한 값
		float Angle;
		// UI에서 화면에 노트를 보여주기 위해 사용하는 값
		FVector2D Direction;
	};

public:
	// UI에서 칼의 회전을 표현하기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetSwordAngle();

	// UI에서 화면에 공격 노트를 보여줄지 판단
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsAttackNoteAlive(int32 index);

	// UI에서 화면에 공격 노트의 위치를 보여주기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetAttackNoteDirection(int32 index);

	// UI에서 화면에 방어 노트를 보여줄지 판단
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsGuardNoteAlive(int32 index);

	// UI에서 화면에 방어 노트의 위치를 보여주기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetGuardNoteDirection(int32 index);

	// UI에서 화면에 스킬 노트를 보여줄지 판단
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	bool IsSkillNoteAlive(int32 index);

	// UI에서 화면에 스킬 노트의 위치를 보여주기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const FVector2D& GetSkillNoteDirection(int32 index);

	// UI에 현재 상태를 표시하기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	EActionState GetCurrentActionState();

	// UI에 현재 무기의 파워를 표시하기 위해 참조
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentSwordPowerRatio();

public:
	// 보스가 죽었을 때 호출되는 콜백
	void OnBossDead();

	// 플레이어가 죽었을 때 호출되는 콜백
	void OnPlayerDead();

	// 노트의 스폰을 시작 시켜야 될 때 호출되는 콜백
	void OnStartSpawnNotes(int32 BossHealth);

private:
	// 키 입력
	void OnActionKeyPressed();
	void OnActionKeyReleased();

private:
	// 플레이어의 스탯을 초기화
	void InitializePlayerStat();

	// 딜리게이터들을 이 함수에서 일괄로 바인딩
	void BindDelegateAll();

	// 새로 스폰시킬 노트가 있는지 확인하여 처리
	void UpdateSpawnNote(float DeltaTime);
	
	// 칼의 회전
	void MoveSword(float DeltaTime);

	// 노트들의 회전
	void MoveNotes(float DeltaTime);
	void MoveNote(float MoveAngle, FNote& Note);

	// 액션키가 입력 되었을 때, 히트될 노트가 있는지 확인하여 처리
	void ActionToAttackNote();

	// 방어 노트와의 상호작용 처리
	void UpdateGuardNote();

	// 이번 페이즈의 모든 노트가 정리 됐는지 확인
	bool IsAllSpawnNotesHide();

	// 행동 상태를 변경하고 그에 따른 처리
	void ChangeActionState(EActionState ActionState);

	// 플레이어의 공격력을 증가
	void IncreaseSwordPower(int32 IncreasePower);

private:
	// 게임 진행을 업데이트 할지 결정
	bool bIsRunning;

	// 칼의 회전 속도, 피격 범위, 최소/최대 파워
	float SwordSpeed;
	float SwordRange;
	int32 SwordMinPower;
	int32 SwordMaxPower;

	// 현재 칼의 회전값
	float SwordAngle;

	// 현재 칼의 공격력
	int32 SwordPower;

	// 보스의 공격력
	int32 BossAttackPower;

	// 공격/방어/스킬 노트들
	FNote AttackNotes[ATTACK_NOTE_COUNT];
	FNote GuardNotes[GUARD_NOTE_COUNT];
	FNote SkillNotes[SKILL_NOTE_COUNT];

	// 액션키가 눌려 있는 상태인지 확인
	bool bIsActionKeyPressed;

	// 현재 액션을 실패로 처리해야 될지?
	bool bShouldCheckMiss;

	// 방어를 시작한 회전값
	float GuardStartAngle;

	// 현재 액션 상태
	EActionState CurrentActionState;

	// 현재 페이즈에 해당하는 노트 데이터
	const FNotePhase* NotePhaseData;

	// 노트를 스폰하기 시작했을 때 부터 누적되어지는 값.
	// 이 값을 기준으로 다음 노트가 스폰될 타이밍이 정해진다.
	float RunningNoteSpawnTime;

	// 현재 스폰되어진 노트의 인덱스
	int32 SpawnTargetNoteIndex;

	// 현재 페이즈의 모든 노트를 스폰했는지 확인
	bool bIsAllSpawnNotesInPhase;

private:
	// 모든 노트가 정리 됐을 때 호출되는 딜리게이트
	AllNotesClearedSignature AllNotesClearedDelegate;

	// 보스를 공격하는 판정이 나왔을 때 호출되는 딜리게이트
	AttackToBossSignature AttackToBossDelegate;

	// 플레이어가 공격을 받는 판정이 나왔을 때 호출되는 딜리게이트
	DamageFromBossSignature DamageFromBossDelegate;

	// 점수를 증가시켜야 될 때 호출되는 딜리게이트
	IncreaseScoreSignature IncreaseScoreDelegate;
};
