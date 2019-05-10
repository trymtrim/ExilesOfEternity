// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "SpellAttributes.h"
#include "UIHandler.generated.h"

UCLASS()
class EXILESOFETERNITY_API UUIHandler : public UObject
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	void SetSpellPanel (UPanelWidget* spellPanel, UUserWidget* baseUI);

	void ReplacePanelChild (UWidget* newWidget, int index);

	UFUNCTION (BlueprintCallable)
	void SwitchPanelPosition (UWidget* widgetOne, UWidget* widgetTwo);

	UFUNCTION (BlueprintCallable)
	bool GetSpellPositionChanged ();

	UFUNCTION (BlueprintCallable)
	void AddSpellToPanel (Spells spell, UWidget* widget);
	UFUNCTION (BlueprintCallable)
	void RemoveSpellFromPanel (Spells spell, UWidget* widget);

	TArray <Spells> GetSpellPanelSpells ();

	UFUNCTION (BlueprintCallable)
	int GetSpellPanelIndex (Spells spell);

private:
	UPanelWidget* _spellPanel = nullptr;
	UUserWidget* _baseUI;

	bool _spellPositionChanged = false;
	
	TArray <Spells> _spellPanelSpells = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};

	int _currentEmptySpellIndex = -1;
	int _currentSpellIndex = 0;
	int _currentSpellAmount = 0;
};
