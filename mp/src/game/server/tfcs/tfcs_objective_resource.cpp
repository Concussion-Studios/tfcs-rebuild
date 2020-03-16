//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tfcs_objective_resource.h"
#include <coordsize.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Datatable
IMPLEMENT_SERVERCLASS_ST( CTFCSObjectiveResource, DT_TFCSObjectiveResource )
END_SEND_TABLE()

BEGIN_DATADESC( CTFCSObjectiveResource )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tfcs_objective_resource, CTFCSObjectiveResource );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCSObjectiveResource::Spawn( void )
{
	BaseClass::Spawn();
}
