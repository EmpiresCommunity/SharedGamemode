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


#include "GameplayScenario.h"
#include "ScenarioInstanceSubsystem.h"
#include "GameplayScenarioAction.h"

UGameplayScenario::UGameplayScenario()
	: Super()
{

}

void UGameplayScenario::PreActivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	ForEachAction_Mutable([&ScenarioSubsystem](UGameplayScenarioAction* Action)
	{
		Action->OnScenarioPreActivated(ScenarioSubsystem);
	});

	
}

void UGameplayScenario::ActivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	ForEachAction_Mutable([&ScenarioSubsystem](UGameplayScenarioAction* Action)
	{
		Action->OnScenarioActivated(ScenarioSubsystem);
	});

}

void UGameplayScenario::DeactivateScenario(UScenarioInstanceSubsystem* ScenarioSubsystem, bool bTearDown)
{
	ForEachAction_Mutable([&ScenarioSubsystem](UGameplayScenarioAction* Action)
	{
		Action->OnScenarioDeactivated(ScenarioSubsystem);
	});
}

void UGameplayScenario::ForEachAction_Mutable(Predicate Predicate)
{
	for (UGameplayScenarioAction* Action : ScenarioActions)
	{
		if(IsValid(Action))
		{
			Predicate(Action);
		}
	}
}

void UGameplayScenario::ForEachAction(ConstPredicate Predicate) const
{
	for (UGameplayScenarioAction* Action : ScenarioActions)
	{
		if (IsValid(Action))
		{
			Predicate(Action);
		}
	}
}

