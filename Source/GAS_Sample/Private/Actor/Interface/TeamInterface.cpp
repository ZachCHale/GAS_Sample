// Copyright ZH_Dev


#include "Actor/Interface/TeamInterface.h"

// Add default functionality here for any ITeamInterface functions that are not pure virtual.
ETeam ITeamInterface::GetTeam()
{
	return ETeam::Neutral;
}

bool ITeamInterface::IsRelativeEnemy(TScriptInterface<ITeamInterface> Source, TScriptInterface<ITeamInterface> Target)
{
	ETeam TargetTeam = Target->GetTeam();
	switch (Source->GetTeam()) {
	case Player:
		return TargetTeam != Player;
		break;
	case Enemy:
		return TargetTeam == Player;
		break;
	case Neutral:
		return false;
		break;
	}
	return false;
}
