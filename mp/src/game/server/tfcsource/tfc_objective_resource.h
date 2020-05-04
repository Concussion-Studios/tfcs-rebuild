//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#ifndef TFC_OBJECTIVE_RESOURCE_H
#define TFC_OBJECTIVE_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "team_objectiveresource.h"

class CTFCObjectiveResource : public CBaseTeamObjectiveResource
{
	DECLARE_CLASS( CTFCObjectiveResource, CBaseTeamObjectiveResource );
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Spawn( void );
};

#endif	// TFC_OBJECTIVE_RESOURCE_H

