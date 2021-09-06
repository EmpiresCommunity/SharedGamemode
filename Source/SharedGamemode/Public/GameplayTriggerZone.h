// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTags.h"
#include "GameplayTriggerZone.generated.h"

class ATriggerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayTriggerZoneEvent, AGameplayTriggerZone*, TriggerZone, AActor*, OverlappingActor);

UCLASS()
class SHAREDGAMEMODE_API AGameplayTriggerZone : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayTriggerZone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger")
	TArray<ATriggerBase*> TriggerVolumes;

	UFUNCTION()
	virtual void OnInternalVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnInternalVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintAssignable)
	FGameplayTriggerZoneEvent OnZoneBeginOverlap;
	UPROPERTY(BlueprintAssignable)
	FGameplayTriggerZoneEvent OnZoneEndOverlap;

	UPROPERTY(EditAnywhere, Category="Trigger")
	FGameplayTagContainer TriggerTags;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const 
	{
		TagContainer.AppendTags(TagContainer);
	}


public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TMap<AActor*, int32> OverlappingActors;

};
