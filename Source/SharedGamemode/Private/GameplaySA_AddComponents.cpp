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


#include "GameplaySA_AddComponents.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "ScenarioInstanceSubsystem.h"

void UGameplaySA_AddComponents::OnScenarioActivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	UGameInstance* GameInstance = ScenarioSubsystem->GetGameInstance();

	if (!IsValid(GameInstance))
	{
		return;
	}

	if (UGameFrameworkComponentManager* GFCM = GameInstance->GetSubsystem<UGameFrameworkComponentManager>())
	{
		bool bIsServer = IsRunningDedicatedServer();
#if WITH_EDITOR
		checkSlow(GameInstance->GetWorldContext());
		bIsServer |= GameInstance->GetWorldContext()->RunAsDedicated;
#endif

		//@TODO: GameFeaturePluginEnginePush: RIP listen servers (don't intend to add support for them, but we should surface that and warn if the world is NM_ListenServer or something like that)
		const bool bIsClient = !bIsServer;

		for (const FGameFeatureComponentEntry& Entry : ComponentList)
		{
			const bool bShouldAddRequest = (bIsServer && Entry.bServerComponent) || (bIsClient && Entry.bClientComponent);
			if (bShouldAddRequest)
			{
				if (!Entry.ActorClass.IsNull())
				{
					TSubclassOf<UActorComponent> ComponentClass = Entry.ComponentClass.LoadSynchronous();
					if (ComponentClass)
					{
						ComponentRequestHandles.Add(GFCM->AddComponentRequest(Entry.ActorClass, ComponentClass));
					}
					else if (!Entry.ComponentClass.IsNull())
					{
						//TODO: A log category for this
						UE_LOG(LogTemp, Error, TEXT("[GameFeatureData %s]: Failed to load component class %s. Not applying component."), *GetPathNameSafe(this), *Entry.ComponentClass.ToString());
					}
				}
			}
		}
	}
}

void UGameplaySA_AddComponents::OnScenarioDeactivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	// Releasing the handles will also remove the components from any registered actors too
	ComponentRequestHandles.Empty();
}
