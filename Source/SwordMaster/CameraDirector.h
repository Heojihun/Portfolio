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
	// 다음 카메라로 전환
	void ChangeToNextCamera();

private:
	// 카메라 액터들
	UPROPERTY(EditAnywhere, Category = "Camera")
	TArray<AActor*> CameraActors;

	// 이 시간이 지난 후 다음 카메라로 전환
	UPROPERTY(EditAnywhere, Category = "Camera")
	float TimeBetweenCameraChanges;

	// 카메라 전환시 블랜드 되는 시간
	UPROPERTY(EditAnywhere, Category = "Camera")
	float SmoothBlendTime;

private:
	// 다음 카메라로 전환할 시간이 됐는지 체크하기 위한 값
	float TimeToNextCameraChange;

	// 현재 화면을 보여주는 카메라의 인덱스
	int32 CurrentViewCameraIndex;
};
