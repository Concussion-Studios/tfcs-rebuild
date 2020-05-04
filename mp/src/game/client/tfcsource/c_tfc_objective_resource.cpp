//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC's objective resource, transmits all objective states to players
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_tfc_objective_resource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_TFCObjectiveResource, DT_TFCObjectiveResource, CTFCObjectiveResource )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCObjectiveResource::C_TFCObjectiveResource()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCObjectiveResource::~C_TFCObjectiveResource()
{
}