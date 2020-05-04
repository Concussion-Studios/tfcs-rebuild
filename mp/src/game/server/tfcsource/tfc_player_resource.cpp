//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC CPlayerResource
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tfc_player.h"
#include "player_resource.h"
#include "tfc_player_resource.h"
#include <coordsize.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Datatable
IMPLEMENT_SERVERCLASS_ST(CTFCPlayerResource, DT_TFCPlayerResource)
	SendPropArray3( SENDINFO_ARRAY3(m_iPlayerClass), SendPropInt( SENDINFO_ARRAY(m_iPlayerClass), 4 ) ),
END_SEND_TABLE()

BEGIN_DATADESC( CTFCPlayerResource )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tfc_player_manager, CTFCPlayerResource );

CTFCPlayerResource::CTFCPlayerResource( void )
{
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCPlayerResource::UpdatePlayerData( void )
{
	int i;

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CTFCPlayer *pPlayer = (CTFCPlayer*)UTIL_PlayerByIndex( i );
		if ( pPlayer && pPlayer->IsConnected() )
			m_iPlayerClass.Set( i, pPlayer->m_Shared.PlayerClass() );
	}

	BaseClass::UpdatePlayerData();
}

void CTFCPlayerResource::Spawn( void )
{
	int i;

	for ( i=0; i < MAX_PLAYERS+1; i++ )
		m_iPlayerClass.Set( i, PLAYERCLASS_UNDEFINED );

	BaseClass::Spawn();
}