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
	// ��� ��Ʈ�� ���� ���� ��, ���Ӱ� ��Ʈ���� �������Ѿ� �ȴٴ� ���� �˷��ش�.
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

		// ������ �׾����� �˸���.
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

		// �÷��̾ �׾����� �˸���.
		PlayerDeadDelegate.Broadcast();
	}
}

// ���� ���� �����͸� �����ͼ� �ʱ�ȭ�ϰ�, ��Ʈ���� ������Ű��� �˸���.
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
	// ���� ��忡�� �¸� / �й迡 ���� �˷��� ��������Ʈ ����
	AGamePlayGameMode* GameMode = dynamic_cast<AGamePlayGameMode*>(GetWorld()->GetAuthGameMode());
	BossDeadDeleate.AddUObject(GameMode, &AGamePlayGameMode::OnBossDead);
	PlayerDeadDelegate.AddUObject(GameMode, &AGamePlayGameMode::OnPlayerDead);

	// �÷��̾� ��Ʈ�ѷ��� �۵��� �����ϱ� ���� ��������Ʈ ����
	AGamePlayPlayerController* PlayerController = dynamic_cast<AGamePlayPlayerController*>(GetWorld()->GetFirstPlayerController());
	BossDeadDeleate.AddUObject(PlayerController, &AGamePlayPlayerController::OnBossDead);
	PlayerDeadDelegate.AddUObject(PlayerController, &AGamePlayPlayerController::OnPlayerDead);
	StartSpawnNotesDelegate.BindUObject(PlayerController, &AGamePlayPlayerController::OnStartSpawnNotes);
}
