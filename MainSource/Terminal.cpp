// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminal.h"
#include "Engine/StaticMesh.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "PlayerControllerMaster.h"

// Sets default values
ATerminal::ATerminal()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	pMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>meshFinder(TEXT("StaticMesh'/Game/Assets/Models/TerminalModel.TerminalModel'"));

	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0,0,0));
		pMeshComponent->SetRelativeRotation(FRotator(0,0,0));
	}

	pMeshComponent->SetNotifyRigidBodyCollision(true);

	power = 195.f;
	bActive = false;
	bCanShoot = true;
	collided = false;
	build = false;
	mult = 1.0f;
	lives = 3;
}

// Called when the game starts or when spawned
void ATerminal::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Build Cannon Power
		if (build)
		{
			power += .8f * mult;

			if (power > 320.f)
			{
				power = 320.;
				mult = -1.0f;
			}

			if (power < 195.f)
			{
				power = 195.f;
				mult = 1.0f;
			}
		}
}

void ATerminal::Hit()
{
	--lives;

	if (lives <= 0)
		lives = 0;

	if (lives == 0)
	{
		Unpossess();
	}

}

void ATerminal::RotateLeft()
{
	pCannon->RotateLeft();
}
void ATerminal::RotateRight()
{
	pCannon->RotateRight();
}
void ATerminal::StopRotating()
{
	pCannon->StopRotating();
}

void ATerminal::ShootCannon()
{
	if (bCanShoot)
	{
		build = false;
		bCanShoot = false;
		pCannon->FireAction(power);
		power = 195.f;

		GetWorldTimerManager().SetTimer(tTimeBetweenShots, this, &ATerminal::ResetFire, 3.f, false);
	}
}

void ATerminal::BuildPower()
{
	if(bCanShoot)
		build = true;
}

void ATerminal::Unpossess()
{
	/*APlayerController* controller = UGameplayStatics::GetPlayerController(this,playerId);
	controller->bAutoManageActiveCameraTarget = false;
	controller->Possess(pPawn);
	pPawn = nullptr;
	bActive = false;*/

	if (pPawn)
	{

		APlayerControllerMaster* player = Cast<APlayerControllerMaster>(pPawn);
		if (player)
		{
			player->bUsing = false;
		}

		APlayerController* controller = UGameplayStatics::GetPlayerController(this, playerId);
		controller->bAutoManageActiveCameraTarget = false;
		controller->Possess(pPawn);
		pPawn = nullptr;
		bActive = false;

	}

}

// Called to bind functionality to input
void ATerminal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Pressed, this, &ATerminal::RotateRight);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Pressed, this, &ATerminal::RotateLeft);
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Released, this, &ATerminal::StopRotating);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Released, this, &ATerminal::StopRotating);
	PlayerInputComponent->BindAction("CannonFire", IE_Pressed, this, &ATerminal::BuildPower);
	PlayerInputComponent->BindAction("CannonFire", IE_Released, this, &ATerminal::ShootCannon);
	PlayerInputComponent->BindAction("Possess", IE_Pressed, this, &ATerminal::Unpossess);
}

void ATerminal::ResetFire()
{
	bCanShoot = true;
}

