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


#include "Components/GamestateScenarioComponent.h"
#include "ScenarioInstanceSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "GameplayScenario.h"

UGamestateScenarioComponent::UGamestateScenarioComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Scenarios.ScenarioComp = this;
	SetIsReplicatedByDefault(true);
}

void UGamestateScenarioComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGamestateScenarioComponent, Scenarios);
}


void UGamestateScenarioComponent::OnRegister()
{
	Super::OnRegister();

	if (IsValid(GetGameInstance<UGameInstance>()))
	{
		if (UScenarioInstanceSubsystem* Subsys = GetGameInstance<UGameInstance>()->GetSubsystem<UScenarioInstanceSubsystem>())
		{
			Subsys->OnScenarioActivated.AddDynamic(this, &ThisClass::OnScenarioActivated);
			Subsys->OnScenarioDeactivated.AddDynamic(this, &ThisClass::OnScenarioDeactivated);

			//Gather any activated scenarios and add them to the list.
			for (UGameplayScenario* Scenario : Subsys->ActiveScenarios)
			{
				OnScenarioActivated(Scenario);
			}
		}
	}
}

void UGamestateScenarioComponent::OnScenarioActivated(UGameplayScenario* Scenario)
{
	FGameplayScenarioNetworkArrayItem Item;
	Item.Scenario = Scenario;

	Scenarios.Items.Add(Item);
	Scenarios.MarkArrayDirty();
	Scenarios.MarkItemDirty(Item);
}

void UGamestateScenarioComponent::OnScenarioDeactivated(UGameplayScenario* Scenario)
{
	Scenarios.Items.RemoveAll([Scenario](const FGameplayScenarioNetworkArrayItem& Item)
		{
			if (Item.Scenario == Scenario)
			{
				return true;
			}
			return false;
		});
	Scenarios.MarkArrayDirty();
}


void UGamestateScenarioComponent::ActivateScenarioLocally(UGameplayScenario* Scenario)
{
	if (UScenarioInstanceSubsystem* Subsys = GetGameInstance<UGameInstance>()->GetSubsystem<UScenarioInstanceSubsystem>())
	{
		Subsys->ActivateScenario(Scenario, false);
	}
}

void UGamestateScenarioComponent::DeactivateScenarioLocally(UGameplayScenario* Scenario)
{
	if (UScenarioInstanceSubsystem* Subsys = GetGameInstance<UGameInstance>()->GetSubsystem<UScenarioInstanceSubsystem>())
	{
		Subsys->DeactivateScenario(Scenario);
	}
}

void FGameplayScenarioNetworkArrayItem::PreReplicatedRemove(const struct FGameplayScenarioNetworkArray& InArraySerializer)
{
	if (IsValid(InArraySerializer.ScenarioComp))
	{
		InArraySerializer.ScenarioComp->DeactivateScenarioLocally(Scenario);
		PrevScenario = nullptr;
	}	
}

void FGameplayScenarioNetworkArrayItem::PostReplicatedAdd(const struct FGameplayScenarioNetworkArray& InArraySerializer)
{
	if (IsValid(InArraySerializer.ScenarioComp))
	{
		InArraySerializer.ScenarioComp->ActivateScenarioLocally(Scenario);
		PrevScenario = Scenario;
	}
}

void FGameplayScenarioNetworkArrayItem::PostReplicatedChange(const struct FGameplayScenarioNetworkArray& InArraySerializer)
{
	if (IsValid(InArraySerializer.ScenarioComp))
	{
		UGameplayScenario* Prev = PrevScenario.Get();
		PrevScenario = Scenario;

		if (IsValid(Prev))
		{
			InArraySerializer.ScenarioComp->DeactivateScenarioLocally(Prev);
		}
		if (IsValid(Scenario))
		{
			InArraySerializer.ScenarioComp->ActivateScenarioLocally(Scenario);
		}	
		
	}
}
