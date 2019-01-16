// Copyright 2018 Allegorithmic Inc. All rights reserved.
// File: SubstanceSettings.cpp

#include "SubstanceSettings.h"
#include "SubstanceCorePrivatePCH.h"
#include "SubstanceInstanceFactory.h"

USubstanceSettings::USubstanceSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, DefaultSubstanceOutputSizeX(EDefaultSubstanceTextureSize::SIZE_1024)
	, DefaultSubstanceOutputSizeY(EDefaultSubstanceTextureSize::SIZE_1024)
	, MaxAsyncSubstancesRenderedPerFrame(10)
	, SubstanceEngine(SET_CPU)
	, AsyncLoadMipClip(3)
	, MemoryBudgetMb(512)
	, CPUCores(32)
{
}
