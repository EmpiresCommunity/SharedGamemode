# Shared Gamemode
### An Unreal Engine Game Feature Plugin for gameplay scenarios

Shared Gamemode creates a modular Gameplay Scenario system that allows for the construction and streaming in of multiple maps, gamemode components, and other elements 

## Quickstart

This is a game feature plugin, and only works in UE 4.27+ (UE5 Early Access has the same plugin and codebase, but is not asset-compatible with 4.27 so the data asset wont load).   

Drop this in your Plugins/GameFeatures/ folder.  It will auto activate.

From there, you can create additional game feature plugins that rely on this one.  In those plugins, you can create a `Gameplay Scenario` data asset, and set it up.



## Using Scenarios

Gameplay Scenarios are exposed to the Asset Registry.  If you already have your own mechanism to query asset tags, the three tags exposed to the asset manager are `Name`, `Description`, and `bTopLevel`.  A "Top Level" Scenario should be how you display scenarios to users.  

If you don't have a mechanism to query asset tags, you can use this:

```cpp
//In some BlueprintFunctionLibrary header:
	UFUNCTION(BlueprintPure, Category = "Assets")
	static bool GetAssetTagAsString(FPrimaryAssetId Asset, FName Tag, FString& OutString);

	UFUNCTION(BlueprintPure, Category = "Assets")
	static bool GetAssetTagAsText(FPrimaryAssetId Asset, FName Tag, FText& OutText);

	UFUNCTION(BlueprintPure, Category = "Assets")
	static bool GetAssetTagAsBool(FPrimaryAssetId Asset, FName Tag, bool& OutBool);

//In that library's cpp:

    bool UYourBPFunctionLibrary::GetAssetTagAsString(FPrimaryAssetId Asset, FName Tag, FString& OutString)
    {
        UAssetManager& LocalManager = UAssetManager::Get();

        FAssetData AssetData;

        if (!LocalManager.GetPrimaryAssetData(Asset, AssetData))
        {
            return false;
        }

        bool bFound = AssetData.GetTagValue<FString>(Tag, OutString);

        return bFound;
    }

    bool UYourBPFunctionLibrary::GetAssetTagAsText(FPrimaryAssetId Asset, FName Tag, FText& OutText)
    {
        UAssetManager& LocalManager = UAssetManager::Get();

        FAssetData AssetData;

        if (!LocalManager.GetPrimaryAssetData(Asset, AssetData))
        {
            return false;
        }

        bool bFound = AssetData.GetTagValue<FText>(Tag, OutText);

        return bFound;
    }

    bool UYourBPFunctionLibrary::GetAssetTagAsBool(FPrimaryAssetId Asset, FName Tag, bool& OutBool)
    {
        UAssetManager& LocalManager = UAssetManager::Get();

        FAssetData AssetData;

        if (!LocalManager.GetPrimaryAssetData(Asset, AssetData))
        {
            return false;
        }

        bool bFound = AssetData.GetTagValue<bool>(Tag, OutBool);

        return bFound;
    }
```

Just use the asset manager to get all `GameplayScenario` assets, and query for their tags in your UI to display them.

To activate a scenario, call the `StartScenario` console command (you can do this from blueprint).  Pass in the FPrimaryAssetId for the scenario as the first parameter and the system will take over from there.  If you are trying to activate a scenario from C++, you need to set the PendingScenario on `UScenarioInstanceSubsystem`, then call `TransitionToPendingScenario()`.  This will wipe all active scenarios.  


## What is a Scenario anyway?

Scenarios are an asset that can be activated to provide gameplay functionality to a level.  In Empires, we have multiple gamemodes (Commander, Conquest, etc) with multiple maps (Canyon, BaenIsland, etc).  We also have a bunch of lighting types (Day, Night, Late Afternoon, Dynamic, etc).  Combining these options to present to users was a challenge, and we added to that challenge by allowing moddable Gamemodes, Maps, and Atmospheres.  

So, that's where Scenarios come in.  Scenarios allow us to select a Terrain, an Atmosphere Layer, and any number of Gamemode elements to combine into one "Map" experience that a player will experience.  

Scenarios have a list of Actions that are taken whe the scenario is made active.  Actions include:

* `ActivateScenario` - Activate another scenario, allowing for scenario composition
* `DeactivateScenario` - Deactivate another scenario by gameplay tag query
* `ChangeMap` - Transition the game/server to another map.  This clears all other scenarios when activated
* `StreamLevelInstance` - Streams in a LevelInstanceDynamic for a given list of levels. 
* `AddComponents` - Adds components to the Modular Gameplay Framework subsystem, so actors registered and listening for components will receive them
* (In Progress) `AddGASPrimitives` - Adds GAS Primitives (Abilities, AttributeSets, and GameplayEffects) to Actors that are listening for them

In Empires, we use Scenario Composition to create our map experiences.  For example, we have the following layout split into 3 different game feature plugins:

```
SC_BaenIslandCQDay (in GameFeatures/Scenarios/BaenIslandConquest)
  Actions: [
    ActivateScenario: SC_BaenIslandDay
    ActivateScenario: SC_Conquest
    StreamLevelInstance: [ L_BaenIslandCQ ]
  ]

SC_BaenIslandDay (in GameFeatures/Terrain/BaenIslandConquest)
  Actions: [
    ChangeMap: L_BaenIsland_Terrain
    StreamLevelInstance: [ L_BaenIsland_Atmo_Day ]
  ]

SC_Conquest (in GameFeatures/Gamemode)
  Actions: [
      ApplyGASPrimitives: {To: TeamState, AddAttributeSets: [Tickets], AddAbilities: [BP_SubtractTicketOnDeath_GA, BP_ConquestVictory_GA] }
  ]
```

## Future

* This plugin will be updated to support UE5 when it releases and Empires updates to it.  
* We will be adding better GAS support for applying GAS stuff to actors with an ASC.
* Probably more dynamic scenario support.  Being able to activate and deactivate things as needed would be nice.

