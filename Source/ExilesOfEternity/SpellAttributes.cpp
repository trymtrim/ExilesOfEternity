// Copyright Sharp Raccoon 2019.

#include "SpellAttributes.h"

TMap <Spells, Spell> USpellAttributes::spellMap = USpellAttributes::InitializeSpellAttributes ();

TMap <Spells, Spell> USpellAttributes::InitializeSpellAttributes ()
{
	//TODO: Load from file or database
	
	TMap <Spells, Spell> map;

	int spellAmount = 3;

	for (int i = 0; i < spellAmount; i++)
	{
		Spell spell;

		spell.Name = "ExampleSpellName";
		spell.Type = SpellTypes (1); //PROJECTION_SPELL - Use int or string
		spell.Damage = 40.0f;
		spell.Range = 2000.0f;

		map.Add (Spells (i), spell);
	}

	return map;
}

SpellTypes USpellAttributes::GetType (Spells spell)
{
	return spellMap [spell].Type;
}

float USpellAttributes::GetDamage (Spells spell)
{
	return spellMap [spell].Damage;
}

float USpellAttributes::GetRange (Spells spell)
{
	return spellMap [spell].Range;
}
