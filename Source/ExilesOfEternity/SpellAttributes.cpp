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

TMap <Items, Item> USpellAttributes::_itemMap = USpellAttributes::InitializeItemAttributes ();
int USpellAttributes::_itemCount = 0;

TMap <Items, Item> USpellAttributes::InitializeItemAttributes ()
{
	TMap <Items, Item> map;
	return map;
}

void USpellAttributes::LoadSpells (UDataTable* spellDataTable, UDataTable* itemDataTable)
{
	//LOAD SPELLS

	//Reset spell count
	_spellCount = 0;

	//Load spell data table
	UDataTable* SpellStatsDataTable = spellDataTable; //FindObject <UDataTable> (ANY_PACKAGE, TEXT ("DataTable'/Game/Miscellaneous/DataTables/Spell_Table.Spell_Table'"));

	//Declare map
	TMap <Spells, Spell> map;
	
	//Add emtpy spell to the map (NOT USED?)
	Spell emptySpell;

	emptySpell.Name = "Empty";
	emptySpell.Type = SpellTypes (0);

	//Iterate through all rows in the data table and add the spell attributes to the map
	int currentRow = 1;

	for (auto it : SpellStatsDataTable->GetRowMap ())
	{
		FSpellStats* stats = (FSpellStats*) (it.Value);

		Spell spell;

		spell.Name = stats->Name;
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

	//LOAD ITEMS

	//Reset item count
	_itemCount = 0;

	//Load item data table
	UDataTable* ItemStatsDataTable = itemDataTable;

	//Declare map
	TMap <Items, Item> itemMap;

	//Iterate through all rows in the data table and add the item attributes to the map
	int currentItemRow = 1;

	for (auto it : ItemStatsDataTable->GetRowMap ())
	{
		FItemStats* stats = (FItemStats*) (it.Value);

		Item item;

		item.Name = stats->Name;
		item.Stone = stats->Stone;
		item.UseTime = stats->UseTime;
		item.Duration = stats->Duration;
		item.Icon = stats->Icon;
		item.Tooltip = stats->Tooltip;
		item.ItemBlueprint = stats->ItemBlueprint;

		itemMap.Add (Items (currentItemRow), item);

		currentItemRow++;

		//Update item count
		_itemCount++;
	}

	_itemMap = itemMap;
}

FString USpellAttributes::GetName (Spells spell)
{
	return _spellMap [spell].Name;
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

FString USpellAttributes::GetItemName (Items item)
{
	return _itemMap [item].Name;
}

bool USpellAttributes::GetItemStone (Items item)
{
	return _itemMap [item].Stone;
}

float USpellAttributes::GetItemUseTime (Items item)
{
	return _itemMap [item].UseTime;
}

float USpellAttributes::GetItemDuration (Items item)
{
	return _itemMap [item].Duration;
}

UTexture2D * USpellAttributes::GetItemIcon (Items item)
{
	return _itemMap [item].Icon;
}

FString USpellAttributes::GetItemTooltip (Items item)
{
	return _itemMap [item].Tooltip;
}

TSubclassOf <AActor> USpellAttributes::GetItemBlueprint (Items item)
{
	return _itemMap [item].ItemBlueprint;
}

int USpellAttributes::GetItemCount ()
{
	return _itemCount;
}
