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


#include "GameplayEventMessageBus.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UGameplayEventMessageBus::NotifyGameplayEvent(FGameplayTag Event, const FGameplayEventData& EventData)
{
	//Notify all the event recievers that this event has happened
	for (AActor* EventReciever : GameplayEventRecievers)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EventReciever, Event, EventData);
	}
}

void UGameplayEventMessageBus::RegisterActorForGameplayEvents(AActor* Actor)
{
	if (IsValid(Actor))
	{
		GameplayEventRecievers.AddUnique(Actor);
		Actor->OnDestroyed.AddDynamic(this, &ThisClass::RemoveActorForGameplayEvents);
	}
}

void UGameplayEventMessageBus::RemoveActorForGameplayEvents(AActor* Actor)
{
	Actor->OnDestroyed.RemoveDynamic(this, &ThisClass::RemoveActorForGameplayEvents);
	GameplayEventRecievers.Remove(Actor);
}

