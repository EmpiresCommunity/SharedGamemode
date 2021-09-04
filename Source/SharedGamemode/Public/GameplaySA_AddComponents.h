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
#include "GameplayScenarioAction.h"
#include "GameFeatureAction_AddComponents.h"
#include "GameplaySA_AddComponents.generated.h"

/**
 * 
 */
UCLASS()
class SHAREDGAMEMODE_API UGameplaySA_AddComponents : public UGameplayScenarioAction
{
	GENERATED_BODY()
public: 
	/** List of components to add to gameplay actors when this game feature is enabled */
	UPROPERTY(EditAnywhere, Category = "Components", meta = (TitleProperty = "ComponentClass"))
		TArray<FGameFeatureComponentEntry> ComponentList;

	TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequestHandles;

	virtual void OnScenarioActivated(UScenarioInstanceSubsystem* ScenarioSubsystem) override;

	virtual void OnScenarioDeactivated(UScenarioInstanceSubsystem* ScenarioSubsystem) override;
};
