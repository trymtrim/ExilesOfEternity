// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "SpellAttributes.generated.h"

UENUM (BlueprintType)
enum Spells
{
	EMPTY,
	FLAMESTRIKE,
	SHIELD,
	BLINK,
	CONTAINMENT_FENCE,
	CYCLONE,
	MEDITATE,
	FROSTBOLT,
	GLACIAL_FORCE
};

UENUM (BlueprintType)
enum CharacterSpells
{
	BASIC,
	ULTIMATE
};

UENUM (BlueprintType)
enum SpellAnimations
{
	PROJECTION_UPWARDS_ANIMATION,
	PROJECTION_DOWNWARDS_ANIMATION,
	PROJECTILE_ANIMATION,
	SELF_USE_ANIMATION,
	CHANNEL_SELF_USE_ANIMATION
};

UENUM (BlueprintType)
enum SpellTypes
{
	PROJECTILE,
	PROJECTION,
	SELF_USE
};

struct Spell
{
	FString Name;
	SpellTypes Type;
	TArray <float> Cooldown;
	TArray <float> Damage;
	TArray <float> Range;
	TArray <float> Radius;
	TArray <float> Duration;
	bool GlobalCooldown;
	bool UsableWhileMoving;
	SpellAnimations Animation;
	UTexture2D* Icon;
	FColor IconColor;
	TArray <FString> Tooltip;
	TSubclassOf <AActor> SpellCapsule;
};

USTRUCT (BlueprintType)
struct FSpellStats : public FTableRowBase
{
	GENERATED_BODY ()

	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte <Spells> Spell;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	FString Name;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte <SpellTypes> Type;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TArray <float> Cooldown;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TArray <float> Damage;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TArray <float> Range;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TArray <float> Radius;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TArray <float> Duration;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	bool GlobalCooldown;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	bool UsableWhileMoving;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte <SpellAnimations> Animation;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	UTexture2D* Icon;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	FColor IconColor;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = true))
	TArray <FString> Tooltip;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf <AActor> SpellCapsule;
};

UENUM (BlueprintType)
enum Items
{
	EMPTY_ITEM,
	HEALTH_POTION,
	RECHARGE_POTION,
	CLOAK_ELIXIR,
	DEFENSE_ELIXIR,
	TELEPORTATION_RUNE,
	LAUNCH_RUNE,
	SOUL_STONE,
	TRACKING_STONE,
	HUNTING_STONE,
	CONSTITUTION_STONE,
	LIFESTEAL_STONE
};

struct Item
{
	FString Name;
	bool Stone;
	float UseTime;
	float Duration;
	UTexture2D* Icon;
	FString Tooltip;
	TSubclassOf <AActor> ItemBlueprint;
};

USTRUCT (BlueprintType)
struct FItemStats : public FTableRowBase
{
	GENERATED_BODY ()

	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte <Items> Item;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	FString Name;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	bool Stone;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float UseTime;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float Duration;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	UTexture2D* Icon;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = true))
	FString Tooltip;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf <AActor> ItemBlueprint;
};

UCLASS()
class EXILESOFETERNITY_API USpellAttributes : public UObject
{
	GENERATED_BODY ()
	
public:
	UFUNCTION (BlueprintCallable)
	static void LoadSpells (UDataTable* spellDataTable, UDataTable* itemDataTable);

	UFUNCTION (BlueprintCallable)
	static FString GetName (Spells spell);
	UFUNCTION (BlueprintCallable)
	static SpellTypes GetType (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetCooldown (Spells spell, int rank);
	UFUNCTION (BlueprintCallable)
	static float GetDamage (Spells spell, int rank);
	UFUNCTION (BlueprintCallable)
	static float GetRange (Spells spell, int rank);
	UFUNCTION (BlueprintCallable)
	static float GetRadius (Spells spell, int rank);
	UFUNCTION (BlueprintCallable)
	static float GetDuration (Spells spell, int rank);
	static bool GetGlobalCooldown (Spells spell);
	UFUNCTION (BlueprintCallable)
	static bool GetUsableWhileMoving (Spells spell);
	UFUNCTION (BlueprintCallable)
	static SpellAnimations GetAnimation (Spells spell);
	UFUNCTION (BlueprintCallable)
	static UTexture2D* GetIcon (Spells spell);
	UFUNCTION (BlueprintCallable)
	static FString GetTooltip (Spells spell, int rank);
	UFUNCTION (BlueprintCallable)
	static FColor GetIconColor (Spells spell);
	static TSubclassOf <AActor> GetSpellCapsule (Spells spell);

	static int GetSpellCount ();

	UFUNCTION (BlueprintCallable)
	static FString GetItemName (Items item);
	UFUNCTION (BlueprintCallable)
	static bool GetItemStone (Items item);
	UFUNCTION (BlueprintCallable)
	static float GetItemUseTime (Items item);
	UFUNCTION (BlueprintCallable)
	static float GetItemDuration (Items item);
	UFUNCTION (BlueprintCallable)
	static UTexture2D* GetItemIcon (Items item);
	UFUNCTION (BlueprintCallable)
	static FString GetItemTooltip (Items item);
	UFUNCTION (BlueprintCallable)
	static TSubclassOf <AActor> GetItemBlueprint (Items item);

	static int GetItemCount ();

private:
	static TMap <Spells, Spell> InitializeSpellAttributes ();
	static TMap <Spells, Spell> _spellMap;

	static int _spellCount;

	static TMap <Items, Item> InitializeItemAttributes ();
	static TMap <Items, Item> _itemMap;

	static int _itemCount;
};
