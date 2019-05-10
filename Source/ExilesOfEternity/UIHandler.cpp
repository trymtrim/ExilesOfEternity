// Copyright Sharp Raccoon 2019.

#include "UIHandler.h"

bool UUIHandler::GetSpellPositionChanged ()
{
	if (_spellPositionChanged)
	{
		_spellPositionChanged = false;
		return true;
	}

	return false;
}

void UUIHandler::SetSpellPanel (UPanelWidget* spellPanel, UUserWidget* baseUI)
{
	_spellPanel = spellPanel;
	_baseUI = baseUI;
}

void UUIHandler::ReplacePanelChild (UWidget* newWidget, int index)
{
	if (_spellPanel == nullptr)
		return;

	_spellPanel->ReplaceChildAt (index, newWidget);
}

void UUIHandler::SwitchPanelPosition (UWidget* widgetOne, UWidget* widgetTwo)
{
	int childOneIndex = _spellPanel->GetChildIndex (widgetOne);
	int childTwoIndex = _spellPanel->GetChildIndex (widgetTwo);

	_spellPanel->ReplaceChildAt (childOneIndex, widgetTwo);
	_spellPanel->ReplaceChildAt (childTwoIndex, widgetOne);

	_spellPositionChanged = true;

	//Update spell list
	_spellPanelSpells.Swap (childOneIndex, childTwoIndex);
}

void UUIHandler::AddSpellToPanel (Spells spell, UWidget* widget)
{
	for (int i = 0; i < _spellPanelSpells.Num (); i++)
	{
		if (_spellPanelSpells [i] == EMPTY)
		{
			ReplacePanelChild (widget, i);

			_spellPanelSpells [i] = spell;
			_spellPositionChanged = true;

			return;
		}
	}
}

void UUIHandler::RemoveSpellFromPanel (Spells spell, UWidget * widget)
{
	for (int i = 0; i < _spellPanelSpells.Num (); i++)
	{
		if (_spellPanelSpells [i] == spell)
		{
			ReplacePanelChild (widget, i);

			_spellPanelSpells [i] = EMPTY;
			_spellPositionChanged = true;

			return;
		}
	}
}

TArray <Spells> UUIHandler::GetSpellPanelSpells ()
{
	return _spellPanelSpells;
}

int UUIHandler::GetSpellPanelIndex (Spells spell)
{
	int spellAmount = 0;

	for (int i = 0; i < _spellPanelSpells.Num (); i++)
	{
		if (_spellPanelSpells [i] != EMPTY)
			spellAmount++;
	}

	if (_currentSpellAmount > spellAmount)
	{
		_currentSpellIndex = 0;
		_currentEmptySpellIndex = -1;
	}

	_currentSpellAmount = spellAmount;

	if (_currentSpellIndex >= 6)
	{
		_currentSpellIndex = 0;
		_currentEmptySpellIndex = -1;
	}

	for (int i = 0; i < _spellPanelSpells.Num (); i++)
	{
		if (spell == EMPTY)
		{
			for (int j = 0; j < _spellPanelSpells.Num (); j++)
			{
				if (_spellPanelSpells [j] == EMPTY && j > _currentEmptySpellIndex)
				{
					_currentEmptySpellIndex = j;

					_currentSpellIndex++;
					return _currentEmptySpellIndex + 1;
				}
			}

			continue;
		}
		else if (_spellPanelSpells [i] == spell)
		{
			_currentSpellIndex++;
			return i + 1;
		}
	}

	return 0;
}
