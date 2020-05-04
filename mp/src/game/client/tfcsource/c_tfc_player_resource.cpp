//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: SDK C_PlayerResource
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_tfc_player_resource.h"
#include "c_tfc_player.h"
#include "tfc_gamerules.h"
#include <shareddefs.h>
#include "hud.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


IMPLEMENT_CLIENTCLASS_DT(C_TFC_PlayerResource, DT_TFCPlayerResource, CTFCPlayerResource)
	RecvPropArray3( RECVINFO_ARRAY(m_iPlayerClass), RecvPropInt( RECVINFO(m_iPlayerClass[0]))),
END_RECV_TABLE()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFC_PlayerResource::C_TFC_PlayerResource()
{
	m_Colors[TEAM_BLUE] = COLOR_BLUE;
	m_Colors[TEAM_RED] = COLOR_RED;
	//m_Colors[TEAM_GREEN] = COLOR_GREEN;
	//m_Colors[TEAM_YELLOW] = COLOR_YELLOW;
	m_Colors[TEAM_SPECTATOR] = COLOR_GREY;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFC_PlayerResource::~C_TFC_PlayerResource()
{
}

int C_TFC_PlayerResource::GetPlayerClass( int iIndex )
{
	if ( !IsConnected( iIndex ) )
		return PLAYERCLASS_UNDEFINED;

	return m_iPlayerClass[iIndex];
}

C_TFC_PlayerResource * TheGameResources( void )
{
	return dynamic_cast<C_TFC_PlayerResource *>(GameResources());
}