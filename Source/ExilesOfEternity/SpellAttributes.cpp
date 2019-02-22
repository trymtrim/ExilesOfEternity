// Copyright Sharp Raccoon 2019.

#include "SpellAttributes.h"
#include "ConstructorHelpers.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"

TMap <Spells, Spell> USpellAttributes::_spellMap = USpellAttributes::InitializeSpellAttributes ();
int USpellAttributes::_spellCount = 0;

TMap <Spells, Spell> USpellAttributes::InitializeSpellAttributes ()
{
	TMap <Spells, Spell> map;
	return map;
}

void USpellAttributes::LoadSpells (UDataTable* spellDataTable)
{
	//Reset spell count
	_spellCount = 0;

	//Load spell data table
	UDataTable* SpellStatsDataTable = spellDataTable; //FindObject <UDataTable> (ANY_PACKAGE, TEXT ("DataTable'/Game/Miscellaneous/DataTables/Spell_Table.Spell_Table'"));

	//Declare map
	TMap <Spells, Spell> map;
	
	//Add emtpy spell to the map
	Spell emptySpell;

	emptySpell.Name = "Empty";
	emptySpell.Type = SpellTypes (0);

	//Iterate through all rows in the data table and add the spell attributes to the map
	int currentRow = 1;

	for (auto it : SpellStatsDataTable->GetRowMap ())
	{
		FSpellStats* stats = (FSpellStats*) (it.Value);

		Spell spell;

		spell.Type = stats->Type;
		spell.Cooldown = stats->Cooldown;
		spell.Damage = stats->Damage;
		spell.Range = stats->Range;
		spell.Radius = stats->Radius;
		spell.Duration = stats->Duration;
		spell.GlobalCooldown = stats->GlobalCooldown;
		spell.UsableWhileMoving = stats->UsableWhileMoving;
		spell.Animation = stats->Animation;
		spell.Icon = stats->Icon;
		spell.IconColor = stats->IconColor;
		spell.Tooltip = stats->Tooltip;
		spell.SpellCapsule = stats->SpellCapsule;

		map.Add (Spells (currentRow), spell);

		currentRow++;

		//Update spell count
		_spellCount++;
	}

	_spellMap = map;
}

SpellTypes USpellAttributes::GetType (Spells spell)
{
	return _spellMap [spell].Type;
}

float USpellAttributes::GetCooldown (Spells spell, int rank)
{
	return _spellMap [spell].Cooldown [rank - 1];
}

float USpellAttributes::GetDamage (Spells spell, int rank)
{
	return _spellMap [spell].Damage [rank - 1];
}

float USpellAttributes::GetRange (Spells spell, int rank)
{
	return _spellMap [spell].Range [rank - 1];
}

float USpellAttributes::GetRadius (Spells spell, int rank)
{
	return _spellMap [spell].Radius [rank - 1];
}

float USpellAttributes::GetDuration (Spells spell, int rank)
{
	return _spellMap [spell].Duration [rank - 1];
}

bool USpellAttributes::GetGlobalCooldown (Spells spell)
{
	return _spellMap [spell].GlobalCooldown;
}

bool USpellAttributes::GetUsableWhileMoving (Spells spell)
{
	return _spellMap [spell].UsableWhileMoving;
}

SpellAnimations USpellAttributes::GetAnimation (Spells spell)
{
	return _spellMap [spell].Animation;
}

UTexture2D* USpellAttributes::GetIcon (Spells spell)
{
	return _spellMap [spell].Icon;
}

FString USpellAttributes::GetTooltip (Spells spell, int rank)
{
	return _spellMap [spell].Tooltip [rank - 1];
}

FColor USpellAttributes::GetIconColor (Spells spell)
{
	return _spellMap [spell].IconColor;
}

TSubclassOf <AActor> USpellAttributes::GetSpellCapsule (Spells spell)
{
	return _spellMap [spell].SpellCapsule;
}

int USpellAttributes::GetSpellCount ()
{
	return _spellCount;
}
