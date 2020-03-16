//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's custom CPlayerResource
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "player_resource.h"
#include "tfcs_player_resource.h"
#include <coordsize.h>

// Datatable
IMPLEMENT_SERVERCLASS_ST( CTFCSPlayerResource, DT_TFCSPlayerResource )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfcs_player_manager, CTFCSPlayerResource );

CTFCSPlayerResource::CTFCSPlayerResource( void )
{
}