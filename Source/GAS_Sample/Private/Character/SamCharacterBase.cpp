// Copyright ZH_Dev

#include "Character/SamCharacterBase.h"

ASamCharacterBase::ASamCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASamCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
