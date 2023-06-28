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
#include "GameplaySC_StreamLevelInstance.generated.h"

class ULevelStreamingDynamic;

/**
 * 
 */
UCLASS()
class SHAREDGAMEMODE_API UGameplaySC_StreamLevelInstance : public UGameplayScenarioAction
{
	GENERATED_BODY()
public:
	
	virtual void OnScenarioActivated(UScenarioInstanceSubsystem* ScenarioSubsystem) override;

	virtual void OnScenarioDeactivated(UScenarioInstanceSubsystem* ScenarioSubsystem, bool bTearDown) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World", meta = (AllowedTypes = "Map"))
		TArray<FPrimaryAssetId> StreamedInLevels;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scenario")
		TArray<ULevelStreamingDynamic*> DynamicLevelStreams;
};
