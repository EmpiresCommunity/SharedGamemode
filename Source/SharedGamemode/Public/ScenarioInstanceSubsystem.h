/*
Copyright 2021 Empires Team

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScenarioInstanceSubsystem.generated.h"

class UGameplayScenario;
class ULevelStreamingDynamic;
class UGameplaySA_ChangeMap;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScenarioDelegate, UGameplayScenario*, Scenario);

/**
 * 
 */
UCLASS()
class SHAREDGAMEMODE_API UScenarioInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UScenarioInstanceSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Scenario")
	virtual void SetPendingScenario(UGameplayScenario* Scenairo);
	UFUNCTION(BlueprintCallable, Category = "Scenario")
	virtual void TransitionToPendingScenario();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario")
	TArray<UGameplayScenario*> ActiveScenarios;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario")
	UGameplayScenario* PendingScenario;

	UPROPERTY()
	UGameplaySA_ChangeMap* TransitionToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Scenario")
	bool bBecomeListenServerFromStandalone;

	UPROPERTY(BlueprintAssignable)
	FScenarioDelegate OnScenarioActivated;
	
	UPROPERTY(BlueprintAssignable)
	FScenarioDelegate OnScenarioDeactivated;

	friend class UGameplaySA_ActivateScenario;
	friend class UGameplaySA_DeactivateScenario;
	friend class UGamestateScenarioComponent;
protected:
	virtual void PreActivateScenario(FPrimaryAssetId ScenarioAsset);
	virtual void PreActivateScenario(UGameplayScenario* Scenario);

	virtual void ActivateScenario(FPrimaryAssetId ScenarioAsset);
	virtual void ActivateScenario(UGameplayScenario* Scenario);

	virtual void DeactivateScenario(UGameplayScenario* Scenario);
	virtual void DeactivateScenario(FPrimaryAssetId ScenarioAsset);

	virtual bool IsScenarioActive(UGameplayScenario* Scenario) const;

	virtual void OnPostLoadMap(UWorld* World);
	virtual void OnPreLoadMap(const FString& MapName);

};
