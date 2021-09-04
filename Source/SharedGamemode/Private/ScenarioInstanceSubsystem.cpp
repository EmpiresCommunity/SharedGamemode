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


#include "ScenarioInstanceSubsystem.h"
#include "Engine.h"

#include "GameplayScenario.h"
#include "GameplayScenarioAction.h"

#include "Engine/AssetManager.h"
#include "Engine/AssetManagerTypes.h"
#include "Engine/LevelStreamingDynamic.h"

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"

#include "GameplaySA_ChangeMap.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameplayScenario, Log, All);

UScenarioInstanceSubsystem::UScenarioInstanceSubsystem()
	: Super()
{
	bBecomeListenServerFromStandalone = true;
	TransitionToWorld = nullptr;
}

void UScenarioInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("StartScenario"),
		TEXT("Begin a Scenario, Changing maps if needed"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateLambda([this](const TArray<FString>& Args, UWorld* World, FOutputDevice& Ar)
			{
				if (Args.Num() != 1)
				{
					Ar.Logf(TEXT("Error loading Scenario: Expected one parameter to StartScenario"));
					return;
				}

				//Parse the primary asset id
				FPrimaryAssetId ScenarioAsset = FPrimaryAssetId::FromString(Args[0]);

				if (!ScenarioAsset.IsValid())
				{
					Ar.Logf(TEXT("Error loading Scenario (%s): Asset Id Is Not Valid"), *ScenarioAsset.ToString());
					return;
				}				
				UAssetManager& Manager = UAssetManager::Get();

				FSoftObjectPath Path = Manager.GetPrimaryAssetPath(ScenarioAsset);

				if (!Path.IsValid())
				{
					Ar.Logf(TEXT("Error loading Scenario (%s): Scenario does not exist"), *ScenarioAsset.ToString());
					return;
				}

				
				auto Handle = Manager.LoadPrimaryAsset(ScenarioAsset);

				if (Handle.IsValid())
				{
					Handle->WaitUntilComplete(10);
				}

				UGameplayScenario* Scenario = Manager.GetPrimaryAssetObject<UGameplayScenario>(ScenarioAsset);

				UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem: Going to Scenario %s"), *GetNameSafe(Scenario));

				SetPendingScenario(Scenario);
				TransitionToPendingScenario();
			}),
		ECVF_Default		
		);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMap);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
}

void UScenarioInstanceSubsystem::SetPendingScenario(UGameplayScenario* Scenairo)
{
	PendingScenario = Scenairo;
}

void UScenarioInstanceSubsystem::TransitionToPendingScenario()
{
	PreActivateScenario(PendingScenario);

	if (IsValid(TransitionToWorld))
	{		
		UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem:Transiting to world %s for scenario %s"), *TransitionToWorld->WorldAsset.ToString(), *GetNameSafe(TransitionToWorld->GetOuter()));

		//Transition to the world
		UGameplaySA_ChangeMap* Transition = TransitionToWorld;
		TransitionToWorld = nullptr;

		Transition->TransitionToWorld(this);

		//Await the level change to try to transition again
		return;
	}

	UGameplayScenario* ScenarioToSwitchTo = PendingScenario;
	PendingScenario = nullptr;

	ActivateScenario(ScenarioToSwitchTo);
}


void UScenarioInstanceSubsystem::PreActivateScenario(FPrimaryAssetId ScenarioAsset)
{
	UAssetManager& Manager = UAssetManager::Get();

	FSoftObjectPath Path = Manager.GetPrimaryAssetPath(ScenarioAsset);

	if (!ensure(Path.IsValid()))
	{
		return;
	}

	UGameplayScenario* Scenario = Manager.GetPrimaryAssetObject<UGameplayScenario>(ScenarioAsset);

	if (!IsValid(Scenario))
	{
		auto LoadHandle = Manager.LoadPrimaryAsset(ScenarioAsset);
		//TODO (when?): Async This.  We need some future/await behavior here.
		if (LoadHandle.IsValid())
		{
			LoadHandle->WaitUntilComplete();
		}

		Scenario = Manager.GetPrimaryAssetObject<UGameplayScenario>(ScenarioAsset);
	}

	if (IsValid(Scenario))
	{
		PreActivateScenario(Scenario);
	}
}

void UScenarioInstanceSubsystem::PreActivateScenario(UGameplayScenario* Scenario)
{
	if (!IsValid(Scenario))
	{
		return;
	}
	if (IsScenarioActive(Scenario))
	{
		return;
	}

	UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem: PreActivating Scenario %s"), *GetNameSafe(Scenario));

	Scenario->PreActivateScenario(this);
}

void UScenarioInstanceSubsystem::ActivateScenario(UGameplayScenario* Scenario)
{
	if (!IsValid(Scenario))
	{
		return;
	}
	if (IsScenarioActive(Scenario))
	{
		return;
	}

	UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem: Activating Scenario %s"), *GetNameSafe(Scenario));

	ActiveScenarios.Add(Scenario);

	//Activate the game actions
	for (UGameplayScenarioAction* Action : Scenario->ScenarioActions)
	{
		Action->OnScenarioActivated(this);
	}
	OnScenarioActivated.Broadcast(Scenario);
}

void UScenarioInstanceSubsystem::ActivateScenario(FPrimaryAssetId ScenarioAsset)
{
	UAssetManager& Manager = UAssetManager::Get();

	FSoftObjectPath Path = Manager.GetPrimaryAssetPath(ScenarioAsset);

	if (!ensure(Path.IsValid()))
	{
		return;
	}

	UGameplayScenario* Scenario = Manager.GetPrimaryAssetObject<UGameplayScenario>(ScenarioAsset);

	if (!IsValid(Scenario))
	{
		auto LoadHandle = Manager.LoadPrimaryAsset(ScenarioAsset);
		//TODO (when?): Async This.  We need some future/await behavior here.
		if (LoadHandle.IsValid())
		{
			LoadHandle->WaitUntilComplete();
		}

		Scenario = Manager.GetPrimaryAssetObject<UGameplayScenario>(ScenarioAsset);
	}

	if (IsValid(Scenario))
	{
		ActivateScenario(Scenario);
	}
}

void UScenarioInstanceSubsystem::DeactivateScenario(UGameplayScenario* Scenario)
{
	if (!IsScenarioActive(Scenario))
	{
		return;
	}

	UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem: Deactivating Scenario %s"), *GetNameSafe(Scenario));

	for (UGameplayScenarioAction* Action : Scenario->ScenarioActions)
	{
		Action->OnScenarioDeactivated(this);
	}
	ActiveScenarios.Remove(Scenario);

	OnScenarioDeactivated.Broadcast(Scenario);
}

void UScenarioInstanceSubsystem::DeactivateScenario(FPrimaryAssetId ScenarioAsset)
{
	UGameplayScenario** SearchedScenario = ActiveScenarios.FindByPredicate([ScenarioAsset](UGameplayScenario* Scenario) {
		if (Scenario->GetPrimaryAssetId() == ScenarioAsset)
		{
			return true;
		}
		return false;
	});

	if (SearchedScenario)
	{
		DeactivateScenario(*SearchedScenario);
	}
}

bool UScenarioInstanceSubsystem::IsScenarioActive(UGameplayScenario* Scenario) const
{
	if (ActiveScenarios.Contains(Scenario))
	{
		return true;
	}

	return false;
}

void UScenarioInstanceSubsystem::OnPostLoadMap(UWorld* World)
{
	if (IsValid(PendingScenario))
	{
		UE_LOG(LogGameplayScenario, Verbose, TEXT("ScenarioSubsystem: After Map Load, Transiting to pending Scenario %s"), *GetNameSafe(PendingScenario));

		TransitionToPendingScenario();
	}
}

void UScenarioInstanceSubsystem::OnPreLoadMap(const FString& MapName)
{
	//If we're about to transition maps, deactivate all scenarios
	for (UGameplayScenario* Scenario : ActiveScenarios)
	{
		DeactivateScenario(Scenario);
	}
}	
