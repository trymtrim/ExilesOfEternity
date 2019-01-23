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
	BLINK
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
	SELF_USE_ANIMATION
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
	float Cooldown;
	float Damage;
	float Range;
	float Radius;
	float Duration;
	bool GlobalCooldown;
	SpellAnimations Animation;
	TSubclassOf <UUserWidget> Icon;
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
	float Cooldown;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float Damage;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float Range;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float Radius;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	float Duration;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	bool GlobalCooldown;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte <SpellAnimations> Animation;
	UPROPERTY (BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf <UUserWidget> Icon;
};

UCLASS()
class EXILESOFETERNITY_API USpellAttributes : public UObject
{
	GENERATED_BODY ()
	
public:
	UFUNCTION (BlueprintCallable)
	static void LoadSpells (UDataTable* spellDataTable);

	UFUNCTION (BlueprintCallable)
	static SpellTypes GetType (Spells spell);
	static float GetCooldown (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetDamage (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetRange (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetRadius (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetDuration (Spells spell);
	static bool GetGlobalCooldown (Spells spell);
	UFUNCTION (BlueprintCallable)
	static SpellAnimations GetAnimation (Spells spell);
	UFUNCTION (BlueprintCallable)
	static TSubclassOf <UUserWidget> GetIcon (Spells spell);

private:
	static TMap <Spells, Spell> InitializeSpellAttributes ();
	static TMap <Spells, Spell> _spellMap;
};
