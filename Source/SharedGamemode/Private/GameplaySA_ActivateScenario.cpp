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


#include "GameplaySA_ActivateScenario.h"
#include "Engine/AssetManager.h"
#include "Engine/AssetManagerTypes.h"

#include "GameplayScenario.h"
#include "ScenarioInstanceSubsystem.h"


void UGameplaySA_ActivateScenario::OnScenarioPreActivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	for (FPrimaryAssetId ScenarioAsset : Scenarios)
	{
		ScenarioSubsystem->PreActivateScenario(ScenarioAsset, false);
	}
}

void UGameplaySA_ActivateScenario::OnScenarioActivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{	
	for (FPrimaryAssetId ScenarioAsset : Scenarios)
	{		
		ScenarioSubsystem->ActivateScenario(ScenarioAsset, false);
	}
}

void UGameplaySA_ActivateScenario::OnScenarioDeactivated(UScenarioInstanceSubsystem* ScenarioSubsystem, bool bTearDown)
{
	//No need to deactivate if we are tearing down.  Teardown will destroy our children anyway.
	if (bTearDown)
	{
		return;
	}
	for (FPrimaryAssetId ScenarioAsset : Scenarios)
	{
		ScenarioSubsystem->DeactivateScenario(ScenarioAsset);
	}
}
