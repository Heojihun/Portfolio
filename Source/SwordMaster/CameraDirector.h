// Copyright 2019 Jihun-Heo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class SWORDMASTER_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// ���� ī�޶�� ��ȯ
	void ChangeToNextCamera();

private:
	// ī�޶� ���͵�
	UPROPERTY(EditAnywhere, Category = "Camera")
	TArray<AActor*> CameraActors;

	// �� �ð��� ���� �� ���� ī�޶�� ��ȯ
	UPROPERTY(EditAnywhere, Category = "Camera")
	float TimeBetweenCameraChanges;

	// ī�޶� ��ȯ�� ���� �Ǵ� �ð�
	UPROPERTY(EditAnywhere, Category = "Camera")
	float SmoothBlendTime;

private:
	// ���� ī�޶�� ��ȯ�� �ð��� �ƴ��� üũ�ϱ� ���� ��
	float TimeToNextCameraChange;

	// ���� ȭ���� �����ִ� ī�޶��� �ε���
	int32 CurrentViewCameraIndex;
};
