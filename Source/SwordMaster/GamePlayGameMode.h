// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameModeBase.h"
#include "GamePlayGameMode.generated.h"

/**
 * 인 게임 플레이 중 메뉴 전환을 담당
 */
UCLASS()
class SWORDMASTER_API AGamePlayGameMode : public ASMGameModeBase
{
	GENERATED_BODY()
	
public:
	AGamePlayGameMode();

	virtual void BeginPlay() override;

protected:
	// 게임을 일시정지 했을 때 보여줄 메뉴 화면
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> PauseWidgetClass;

	// 게임에서 승리 했을 때 보여줄 메뉴 화면
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> GameWinWidgetClass;

	// 게임에서 패배 했을 때 보여줄 메뉴 화면
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> GameLoseWidgetClass;

public:
	// 보스가 죽었을 때 호출되는 콜백
	void OnBossDead();

	// 플레이어가 죽었을 때 호출되는 콜백
	void OnPlayerDead();
};
