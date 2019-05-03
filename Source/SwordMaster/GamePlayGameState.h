// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GamePlayGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(BossDeadSignature);
DECLARE_MULTICAST_DELEGATE(PlayerDeadSignature);
DECLARE_DELEGATE_OneParam(StartSpawnNotesSignature, int32);

/**
 * 보스 액터의 애니메이션을 제어하기 위해 사용하는 상태값
 * TODO: 보스 액터를 C++ 코드로 좀 더 자세하게 제어하게 된다면 그 쪽으로 옮겨야 한다.
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
 * 인 게임 플레이 중 게임 승리, 패배를 판단하기 위한 데이터를 다룬다.
 */
UCLASS()
class SWORDMASTER_API AGamePlayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	// 현재 보스 체력의 백분율을 반환
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentBossHealthRatio();

	// 현재 플레이어 체력의 백분율을 반환
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	float GetCurrentPlayerHealthRatio();

	// 현재 점수를 반환
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	const int32& GetCurrentScore();

	// 현재 보스의 상태를 반환 (보스 액터의 애니메이션에서 사용된다.)
	UFUNCTION(BlueprintCallable, Category = "Boss Action")
	EBossActionState GetCurrentBossActionState();

public:
	// 모든 노트가 정리 됐을 때 호출되는 콜백
	void OnAllNotesCleared();

	// 보스를 공격하는 판정이 발생했을 때 호출되는 콜백
	void OnAttackToBoss(int32 Damage);

	// 플레이어가 피해를 받는 판정이 발생했을 때 호출되는 콜백
	void OnDamageFromBoss(int32 Damage);

public:
	// 스테이지 데이터를 초기화
	void InitializeLevel();

	// 딜리게이터들을 이 함수에서 일괄로 바인딩
	void BindDelegateAll();

private:
	// 현재 보스 체력
	int32 CurrentBossHealth;

	// 최대 보스 체력
	int32 MaxBossHealth;

	// 현재 플레이어 체력
	int32 CurrentPlayerHealth;

	// 최대 플레이어 체력
	int32 MaxPlayerHealth;

	// 현재 점수
	int32 CurrentScore;

	// 현재 보스의 행동 상태
	EBossActionState CurrentBossActionState;

private:
	// 보스가 죽었을 때 알려줄 딜리게이트
	BossDeadSignature BossDeadDeleate;

	// 플레이어가 죽었을 때 알려줄 딜리게이트
	PlayerDeadSignature PlayerDeadDelegate;

	// 현재 페이즈의 노트들을 스폰시키기 시작해될 때를 알려줄 딜리게이트
	StartSpawnNotesSignature StartSpawnNotesDelegate;
};
