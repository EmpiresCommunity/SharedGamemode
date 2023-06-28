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


#include "GameplaySC_StreamLevelInstance.h"

#include "Engine/AssetManager.h"
#include "Engine/AssetManagerTypes.h"
#include "Engine/LevelStreamingDynamic.h"

#include "ScenarioInstanceSubsystem.h"

void UGameplaySC_StreamLevelInstance::OnScenarioActivated(UScenarioInstanceSubsystem* ScenarioSubsystem)
{
	UAssetManager& AssetManager = UAssetManager::Get();
	//Stream in the levels
	for (FPrimaryAssetId AssetId : StreamedInLevels)
	{
		FSoftObjectPath ObjPath = AssetManager.GetPrimaryAssetPath(AssetId);
		if (ObjPath.IsValid())
		{
			TSoftObjectPtr<UWorld> ObjPtr(ObjPath);

			bool bSuccess = false;
			ULevelStreamingDynamic* Streamer = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(ScenarioSubsystem->GetGameInstance(), ObjPtr, FVector::ZeroVector, FRotator::ZeroRotator, bSuccess);

			if (bSuccess)
			{
				Streamer->SetShouldBeLoaded(true);
				DynamicLevelStreams.AddUnique(Streamer);
			}
		}
	}
}

void UGameplaySC_StreamLevelInstance::OnScenarioDeactivated(UScenarioInstanceSubsystem* ScenarioSubsystem, bool bTearDown)
{
	//If we're tearing down the level anyway, no need to unload this. It'll be unloaded with the rest of the level
	if (bTearDown)
	{
		return;
	}
	for (ULevelStreamingDynamic* LevelStream : DynamicLevelStreams)
	{
		LevelStream->SetIsRequestingUnloadAndRemoval(true);
	}
}
