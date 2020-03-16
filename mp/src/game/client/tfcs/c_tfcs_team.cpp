//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Client side C_TFCSTeam class
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "recvproxy.h"
#include "c_tfcs_team.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_TFCSTeam, DT_TFCSTeam, CTFCSTeam )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCSTeam::C_TFCSTeam()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCSTeam::~C_TFCSTeam()
{
}

//-----------------------------------------------------------------------------
// Purpose: Get the localized name for the team
//-----------------------------------------------------------------------------
char* C_TFCSTeam::Get_Name( void )
{
	if ( Q_stricmp( m_szTeamname, "blue" ) == 0 )
		return "BLU";
	else if ( Q_stricmp( m_szTeamname, "red" ) == 0 )
		return "RED";
	else if ( Q_stricmp( m_szTeamname, "green" ) == 0 )
		return "GRN";
	else if ( Q_stricmp( m_szTeamname, "yellow" ) == 0 )
		return "YLW";

	return m_szTeamname;
}

//-----------------------------------------------------------------------------
// Purpose: Get the C_TFCSTeam for the specified team number
//-----------------------------------------------------------------------------
C_TFCSTeam *GetGlobalTFCSTeam( int iTeamNumber )
{
	for ( int i = 0; i < g_Teams.Count(); i++ )
	{
		if ( g_Teams[i]->GetTeamNumber() == iTeamNumber )
			return ( dynamic_cast< C_TFCSTeam* >( g_Teams[i] ) );
	}

	return NULL;
}