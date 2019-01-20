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
	//Update UI
	UUserWidget* newWidgetOne = CreateWidget (_spellPanel, widgetOne->GetClass ());
	UUserWidget* newWidgetTwo = CreateWidget (_spellPanel, widgetTwo->GetClass ());

	int childOneIndex = _spellPanel->GetChildIndex (widgetOne);
	int childTwoIndex = _spellPanel->GetChildIndex (widgetTwo);

	_spellPanel->ReplaceChildAt (childOneIndex, newWidgetTwo);
	_spellPanel->ReplaceChildAt (childTwoIndex, newWidgetOne);

	_spellPositionChanged = true;

	//Update spell list
	_spellPanelSpells.Swap (childOneIndex, childTwoIndex);
}

void UUIHandler::AddSpellToPanel (Spells spell, UWidget* widget)
{
	for (int i = 0; i < _spellPanelSpells.Num (); i++)
	{
		if (_spellPanelSpells [i] == DEFAULT_SPELL)
		{
			ReplacePanelChild (widget, i);

			_spellPanelSpells [i] = spell;
			_spellPositionChanged = true;

			return;
		}
	}
}

TArray <Spells> UUIHandler::GetSpellPanelSpells ()
{
	return _spellPanelSpells;
}
