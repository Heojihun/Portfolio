// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMGameModeBase.generated.h"

/**
 * 모든 게임 모드들의 기본 클래스
 */
UCLASS()
class SWORDMASTER_API ASMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	// UI 메뉴를 전환
	void ChangeMenuWidget(TSubclassOf<class UUserWidget> NewWidgetClass);

protected:
	// 기본적으로 보여줄 UI 메뉴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> StartingWidgetClass;

private:
	// 현재 UI 메뉴
	UUserWidget* CurrentWidget;
};
