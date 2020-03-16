//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#ifndef TFCS_OBJECTIVE_RESOURCE_H
#define TFCS_OBJECTIVE_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "team_objectiveresource.h"

class CTFCSObjectiveResource : public CBaseTeamObjectiveResource
{
	DECLARE_CLASS( CTFCSObjectiveResource, CBaseTeamObjectiveResource );
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Spawn( void );
};

#endif	// TFCS_OBJECTIVE_RESOURCE_H

