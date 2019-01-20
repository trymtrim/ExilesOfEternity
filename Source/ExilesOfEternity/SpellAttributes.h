// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpellAttributes.generated.h"

UENUM (BlueprintType)
enum Spells
{
	DEFAULT_SPELL,
	EXAMPLE_PROJECTING_SPELL
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
	PROJECTION_UPWARDS,
	PROJECTION_DOWNWARDS,
	PROJECTILE
};

enum SpellTypes
{
	PROJECTILE_SPELL,
	PROJECTION_SPELL
};

struct Spell
{
	FString Name;
	SpellTypes Type;
	float Damage;
	float Range;
};

UCLASS()
class EXILESOFETERNITY_API USpellAttributes : public UObject
{
	GENERATED_BODY ()
	
public:
	static SpellTypes GetType (Spells spell);
	
	UFUNCTION (BlueprintCallable)
	static float GetDamage (Spells spell);
	UFUNCTION (BlueprintCallable)
	static float GetRange (Spells spell);

private:
	static TMap <Spells, Spell> InitializeSpellAttributes ();
	static TMap <Spells, Spell> spellMap;
};
