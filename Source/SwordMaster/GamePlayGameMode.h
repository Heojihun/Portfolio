// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameModeBase.h"
#include "GamePlayGameMode.generated.h"

/**
 * �� ���� �÷��� �� �޴� ��ȯ�� ���
 */
UCLASS()
class SWORDMASTER_API AGamePlayGameMode : public ASMGameModeBase
{
	GENERATED_BODY()
	
public:
	AGamePlayGameMode();

	virtual void BeginPlay() override;

protected:
	// ������ �Ͻ����� ���� �� ������ �޴� ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> PauseWidgetClass;

	// ���ӿ��� �¸� ���� �� ������ �޴� ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> GameWinWidgetClass;

	// ���ӿ��� �й� ���� �� ������ �޴� ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> GameLoseWidgetClass;

public:
	// ������ �׾��� �� ȣ��Ǵ� �ݹ�
	void OnBossDead();

	// �÷��̾ �׾��� �� ȣ��Ǵ� �ݹ�
	void OnPlayerDead();
};
