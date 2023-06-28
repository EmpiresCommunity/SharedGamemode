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
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "GameplayScenario.generated.h"

class UGameplayScenarioAction;
class UScenarioInstanceSubsystem;

/**
 * 
 */
UCLASS(BlueprintType)
class SHAREDGAMEMODE_API UGameplayScenario : public UPrimaryDataAsset, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:
	UGameplayScenario();

	//Optional Map.  If set, this will be loaded when the scenario is activated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actions", meta = (AllowedTypes = "Map"), AssetRegistrySearchable)
	FPrimaryAssetId Map;

	//Actions to run when the scenario is activated
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<UGameplayScenarioAction*> ScenarioActions;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", AssetRegistrySearchable)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", AssetRegistrySearchable)
	FText Description;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FGameplayTagContainer ScenarioTags;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const { TagContainer.AppendTags(ScenarioTags); }

	void PreActivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem);
	void ActivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem);
	void DeactivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem, bool bTearDown = false);

	using Predicate = TFunctionRef<void(UGameplayScenarioAction*)>;
	void ForEachAction_Mutable(Predicate Predicate);

	using ConstPredicate = TFunctionRef<void(const UGameplayScenarioAction*)>;
	void ForEachAction(ConstPredicate Predicate) const;
};
