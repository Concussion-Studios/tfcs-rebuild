//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#ifndef C_TFC_OBJECTIVE_RESOURCE_H
#define C_TFC_OBJECTIVE_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "const.h"
#include "c_baseentity.h"
#include <igameresources.h>
#include "c_team_objectiveresource.h"

class C_TFCObjectiveResource : public C_BaseTeamObjectiveResource
{
	DECLARE_CLASS( C_TFCObjectiveResource, C_BaseTeamObjectiveResource );
public:
	DECLARE_CLIENTCLASS();

			C_TFCObjectiveResource();
	virtual ~C_TFCObjectiveResource();
};

inline C_TFCObjectiveResource *TheObjectiveResource()
{
	return static_cast< C_TFCObjectiveResource* >( g_pObjectiveResource );
}

#endif // C_TFC_OBJECTIVE_RESOURCE_H