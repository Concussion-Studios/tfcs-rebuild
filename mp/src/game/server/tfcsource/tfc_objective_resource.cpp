//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tfc_objective_resource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_SERVERCLASS_ST( CTFCObjectiveResource, DT_TFCObjectiveResource )
END_SEND_TABLE()

BEGIN_DATADESC( CTFCObjectiveResource )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tfc_objective_resource, CTFCObjectiveResource );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCObjectiveResource::Spawn( void )
{
	BaseClass::Spawn();
}
