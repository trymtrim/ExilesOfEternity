// Copyright Sharp Raccoon 2019.

#include "SpellAttributes.h"

TMap <Spells, Spell> USpellAttributes::spellMap = USpellAttributes::InitializeSpellAttributes ();

TMap <Spells, Spell> USpellAttributes::InitializeSpellAttributes ()
{
	//TODO: Load from file or database
	
	TMap <Spells, Spell> map;

	int spellAmount = 1;

	for (int i = 0; i < spellAmount + 1; i++)
	{
		Spell spell;

		switch (i)
		{
		case 0:
			spell.Name = "Default Spell";
			spell.Type = SpellTypes (0); //PROJECTION_SPELL - Use int or string
			spell.Cooldown = 0.0f;
			spell.Damage = 0.0f;
			spell.Range = 0.0f;
			break;
		case 1:
			spell.Name = "ExampleSpellName";
			spell.Type = SpellTypes (1); //PROJECTION_SPELL - Use int or string
			spell.Cooldown = 5.0f;
			spell.Damage = 40.0f;
			spell.Range = 2000.0f;
			break;
		case 2:
			break;
		}

		map.Add (Spells (i), spell);
	}

	return map;
}

SpellTypes USpellAttributes::GetType (Spells spell)
{
	return spellMap [spell].Type;
}

float USpellAttributes::GetCooldown (Spells spell)
{
	return spellMap [spell].Cooldown;
}

float USpellAttributes::GetDamage (Spells spell)
{
	return spellMap [spell].Damage;
}

float USpellAttributes::GetRange (Spells spell)
{
	return spellMap [spell].Range;
}
