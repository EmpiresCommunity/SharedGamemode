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
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayEventMessageBus.generated.h"

/**
 * 
 */
UCLASS()
class SHAREDGAMEMODE_API UGameplayEventMessageBus : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Emp|Gamemode")
		virtual void NotifyGameplayEvent(FGameplayTag Event, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "Emp|Gamemode")
		virtual void RegisterActorForGameplayEvents(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Emp|Gamemode")
		virtual void RemoveActorForGameplayEvents(AActor* Actor);

	UPROPERTY()
	TArray<AActor*> GameplayEventRecievers;
};
