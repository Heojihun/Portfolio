// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMGameModeBase.generated.h"

/**
 * ��� ���� ������ �⺻ Ŭ����
 */
UCLASS()
class SWORDMASTER_API ASMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	// UI �޴��� ��ȯ
	void ChangeMenuWidget(TSubclassOf<class UUserWidget> NewWidgetClass);

protected:
	// �⺻������ ������ UI �޴�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Menu Widget")
	const TSubclassOf<UUserWidget> StartingWidgetClass;

private:
	// ���� UI �޴�
	UUserWidget* CurrentWidget;
};
