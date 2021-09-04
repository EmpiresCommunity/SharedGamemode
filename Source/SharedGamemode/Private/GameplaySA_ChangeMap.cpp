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


#include "GameplaySA_ChangeMap.h"
#include "Engine.h"
#include "ScenarioInstanceSubsystem.h"

void UGameplaySA_ChangeMap::OnScenarioPreActivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	UWorld* LoadedWorld = ScenarioSubsystem->GetGameInstance()->GetWorld();
	FName LoadedWorldName = FName(*UWorld::RemovePIEPrefix(LoadedWorld->GetOutermost()->GetName()));

	if (LoadedWorldName != WorldAsset.PrimaryAssetName)
	{
		ScenarioSubsystem->TransitionToWorld = this;
	}
	
}

void UGameplaySA_ChangeMap::TransitionToWorld(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	const UWorld* const World = ScenarioSubsystem->GetGameInstance()->GetWorld();
	//We're already here.  Don't bother transitioning
	if (World->GetPrimaryAssetId() == WorldAsset)
	{
		return;
	}

	const bool bIsClient = World->GetNetMode() == NM_Client;

	//Don't transition if we're the client.  We're probably at this world
	if (bIsClient)
	{
		return;
	}

	const bool bIsDedicatedServer = IsRunningDedicatedServer();
	const bool bIsListenServer = World->GetNetMode() == NM_ListenServer; //TODO: In UE5, this mode goes away.
	const bool bIsStandalone = World->GetNetMode() == NM_Standalone;

	FURL NewMapURL = FURL(*WorldAsset.PrimaryAssetName.ToString());

	//In 4.27, we want to change a StandAlone to ListenServer when transitioning to a scenario.  
	//In UE5, Listen Servers are being removed.  
	if (bIsStandalone)
	{
		NewMapURL.AddOption(TEXT("listen"));
	}

	//Travel to the new scenario world
	if (AGameModeBase* GameMode = World->GetAuthGameMode())
	{
		if (GameMode->CanServerTravel(NewMapURL.ToString(), false))
		{
			GameMode->ProcessServerTravel(NewMapURL.ToString(), false);
		}
	}
}
