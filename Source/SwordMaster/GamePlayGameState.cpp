// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "GamePlayGameState.h"
#include "GamePlayGameMode.h"
#include "GamePlayPlayerController.h"
#include "Engine/World.h"

void AGamePlayGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ASwordMasterGameStateBase::BeginPlay"));

	BindDelegateAll();

	InitializeLevel();
}

float AGamePlayGameState::GetCurrentBossHealthRatio()
{
	float Ratio = (float)CurrentBossHealth / (float)MaxBossHealth;
	return Ratio;
}

float AGamePlayGameState::GetCurrentPlayerHealthRatio()
{
	float Ratio = (float)CurrentPlayerHealth / (float)MaxPlayerHealth;
	return Ratio;
}

const int32& AGamePlayGameState::GetCurrentScore()
{
	return CurrentScore;
}

EBossActionState AGamePlayGameState::GetCurrentBossActionState()
{
	EBossActionState ActionState = CurrentBossActionState;
	CurrentBossActionState = EBossActionState::EIdle;
	return ActionState;
}

void AGamePlayGameState::OnAllNotesCleared()
{
	// 모든 노트가 정리 됐을 때, 새롭게 노트들을 스폰시켜야 된다는 것을 알려준다.
	StartSpawnNotesDelegate.Execute(CurrentBossHealth);
}

void AGamePlayGameState::OnAttackToBoss(int32 Damage)
{
	CurrentBossHealth -= Damage;

	CurrentBossActionState = EBossActionState::EDamaged;

	if (CurrentBossHealth <= 0)
	{
		CurrentBossHealth = 0;
		CurrentBossActionState = EBossActionState::EDead;

		// 보스가 죽었음을 알린다.
		BossDeadDeleate.Broadcast();
	}
}

void AGamePlayGameState::OnDamageFromBoss(int32 Damage)
{
	CurrentPlayerHealth -= Damage;

	CurrentBossActionState = EBossActionState::EAttack;

	if (CurrentPlayerHealth <= 0)
	{
		CurrentPlayerHealth = 0;

		// 플레이어가 죽었음을 알린다.
		PlayerDeadDelegate.Broadcast();
	}
}

// 현재 레벨 데이터를 가져와서 초기화하고, 노트들을 스폰시키라고 알린다.
void AGamePlayGameState::InitializeLevel()
{
	USMGameInstance* GameInstance = Cast<USMGameInstance>(GetGameInstance());
	verify(GameInstance != nullptr);

	const FLevelData* LevelData = GameInstance->GetCurrentLevelData();
	verify(LevelData != nullptr);

	CurrentBossActionState = EBossActionState::EIdle;

	MaxBossHealth = LevelData->BossHealth;
	CurrentBossHealth = MaxBossHealth;
	MaxPlayerHealth = GameInstance->GetCurrentPlayerHealth();
	CurrentPlayerHealth = MaxPlayerHealth;

	StartSpawnNotesDelegate.Execute(CurrentBossHealth);
}

void AGamePlayGameState::BindDelegateAll()
{
	// 게임 모드에게 승리 / 패배에 대해 알려줄 딜리게이트 연결
	AGamePlayGameMode* GameMode = dynamic_cast<AGamePlayGameMode*>(GetWorld()->GetAuthGameMode());
	BossDeadDeleate.AddUObject(GameMode, &AGamePlayGameMode::OnBossDead);
	PlayerDeadDelegate.AddUObject(GameMode, &AGamePlayGameMode::OnPlayerDead);

	// 플레이어 컨트롤러의 작동을 제어하기 위한 딜리게이트 연결
	AGamePlayPlayerController* PlayerController = dynamic_cast<AGamePlayPlayerController*>(GetWorld()->GetFirstPlayerController());
	BossDeadDeleate.AddUObject(PlayerController, &AGamePlayPlayerController::OnBossDead);
	PlayerDeadDelegate.AddUObject(PlayerController, &AGamePlayPlayerController::OnPlayerDead);
	StartSpawnNotesDelegate.BindUObject(PlayerController, &AGamePlayPlayerController::OnStartSpawnNotes);
}
