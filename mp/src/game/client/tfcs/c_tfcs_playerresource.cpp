//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's custom C_PlayerResource
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_tfcs_playerresource.h"
#include "hud.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_TFCS_PlayerResource, DT_TFCSPlayerResource, CTFCSPlayerResource )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCS_PlayerResource::C_TFCS_PlayerResource()
{
	m_Colors[TEAM_UNASSIGNED] = COLOR_SPECTATOR;
	m_Colors[TEAM_SPECTATOR] = COLOR_SPECTATOR;
	m_Colors[TEAM_RED] = COLOR_TFCS_RED;
	m_Colors[TEAM_BLUE] = COLOR_TFCS_BLUE;
	m_Colors[TEAM_GREEN] = COLOR_TFCS_GREEN;
	m_Colors[TEAM_YELLOW] = COLOR_TFCS_YELLOW;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCS_PlayerResource::~C_TFCS_PlayerResource()
{
}