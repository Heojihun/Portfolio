// Copyright 2019 Jihun-Heo. All Rights Reserved.

#include "CameraDirector.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

// Sets default values
ACameraDirector::ACameraDirector()
	: TimeToNextCameraChange(0)
	, CurrentViewCameraIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetViewTarget(CameraActors[CurrentViewCameraIndex]);
	}
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeToNextCameraChange -= DeltaTime;
	if (TimeToNextCameraChange <= 0.f)
	{
		TimeToNextCameraChange += TimeBetweenCameraChanges;

		ChangeToNextCamera();
	}
}

void ACameraDirector::ChangeToNextCamera()
{
	CurrentViewCameraIndex++;
	if (CurrentViewCameraIndex >= CameraActors.Num())
	{
		CurrentViewCameraIndex = 0;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(CameraActors[CurrentViewCameraIndex], SmoothBlendTime);
	}
}