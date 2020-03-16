//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: Entity that propagates general data needed by clients for every player.
//
// $NoKeywords: $
//=============================================================================//
#ifndef C_TFCS_OBJECTIVE_RESOURCE_H
#define C_TFCS_OBJECTIVE_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "const.h"
#include "c_baseentity.h"
#include <igameresources.h>
#include "c_team_objectiveresource.h"

class C_TFCSObjectiveResource : public C_BaseTeamObjectiveResource
{
	DECLARE_CLASS( C_TFCSObjectiveResource, C_BaseTeamObjectiveResource );
public:
	DECLARE_CLIENTCLASS();

					C_TFCSObjectiveResource();
	virtual			~C_TFCSObjectiveResource();
};

inline C_TFCSObjectiveResource *TFCSObjectiveResource()
{
	return static_cast<C_TFCSObjectiveResource*>(g_pObjectiveResource);
}

#endif // C_TFCS_OBJECTIVE_RESOURCE_H
