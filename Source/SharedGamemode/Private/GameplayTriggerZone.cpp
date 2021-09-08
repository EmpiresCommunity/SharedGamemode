// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTriggerZone.h"
#include "Engine/TriggerBase.h"
#include "Engine.h"
#include "Components/GameFrameworkComponentManager.h"

// Sets default values
AGameplayTriggerZone::AGameplayTriggerZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AGameplayTriggerZone::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UGameFrameworkComponentManager* GameFrameworkSubsys = GetGameInstance()->GetSubsystem<UGameFrameworkComponentManager>())
	{
		GameFrameworkSubsys->AddReceiver(this);
	}
}

void AGameplayTriggerZone::OnInternalVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	int32& Overlaps = OverlappingActors.FindOrAdd(OtherActor, 1);
	
	if (Overlaps == 1)
	{
		OnZoneBeginOverlap.Broadcast(this, OtherActor);
	}
	else
	{
		Overlaps += 1;
	}
}

void AGameplayTriggerZone::OnInternalVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappingActors.Contains(OtherActor))
	{
		int32& Overlaps = OverlappingActors[OtherActor];

		Overlaps--;

		if (Overlaps <= 0)
		{
			OnZoneEndOverlap.Broadcast(this, OtherActor);
			OverlappingActors.Remove(OtherActor);
		}
	}
}

// Called when the game starts or when spawned
void AGameplayTriggerZone::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		for (ATriggerBase* TriggerActor : TriggerVolumes)
		{
			TriggerActor->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnInternalVolumeBeginOverlap);
			TriggerActor->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnInternalVolumeEndOverlap);
		}
	}
}

void AGameplayTriggerZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{	
	if (GetLocalRole() == ROLE_Authority)
	{
		for (ATriggerBase* TriggerActor : TriggerVolumes)
		{
			TriggerActor->GetCollisionComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnInternalVolumeBeginOverlap);
			TriggerActor->GetCollisionComponent()->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnInternalVolumeEndOverlap);
		}
	}

	Super::EndPlay(EndPlayReason);
}

