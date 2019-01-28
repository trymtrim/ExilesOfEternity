// Copyright Sharp Raccoon 2019.

#include "SpellAttributes.h"
#include "ConstructorHelpers.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"

TMap <Spells, Spell> USpellAttributes::_spellMap = USpellAttributes::InitializeSpellAttributes ();

TMap <Spells, Spell> USpellAttributes::InitializeSpellAttributes ()
{
	TMap <Spells, Spell> map;
	return map;
}

void USpellAttributes::LoadSpells (UDataTable* spellDataTable)
{
	//Load spell data table
	UDataTable* SpellStatsDataTable = spellDataTable; //FindObject <UDataTable> (ANY_PACKAGE, TEXT ("DataTable'/Game/Miscellaneous/DataTables/Spell_Table.Spell_Table'"));

	//Declare map
	TMap <Spells, Spell> map;
	
	//Add emtpy spell to the map
	Spell emptySpell;

	emptySpell.Name = "Empty";
	emptySpell.Type = SpellTypes (0);
	emptySpell.Cooldown = 0.0f;
	emptySpell.Damage = 0.0f;
	emptySpell.Range = 0.0f;
	emptySpell.Radius = 0.0f;
	emptySpell.Duration = 0.0f;

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

		map.Add (Spells (currentRow), spell);

		currentRow++;
	}

	_spellMap = map;
}

SpellTypes USpellAttributes::GetType (Spells spell)
{
	return _spellMap [spell].Type;
}

float USpellAttributes::GetCooldown (Spells spell)
{
	return _spellMap [spell].Cooldown;
}

float USpellAttributes::GetDamage (Spells spell)
{
	return _spellMap [spell].Damage;
}

float USpellAttributes::GetRange (Spells spell)
{
	return _spellMap [spell].Range;
}

float USpellAttributes::GetRadius (Spells spell)
{
	return _spellMap [spell].Radius;
}

float USpellAttributes::GetDuration (Spells spell)
{
	return _spellMap [spell].Duration;
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

FString USpellAttributes::GetTooltip (Spells spell)
{
	return _spellMap [spell].Tooltip;
}

FColor USpellAttributes::GetIconColor (Spells spell)
{
	return _spellMap [spell].IconColor;
}
