//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tfcs_team.h"
#include "entitylist.h"
#include "util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
//
// TFCS Team tables.
//
IMPLEMENT_SERVERCLASS_ST( CTFCSTeam, DT_TFCSTeam )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfcs_team, CTFCSTeam );

//=============================================================================
//
// TFCS Team Manager Functions.
//
CTFCSTeamManager s_TFCSTeamManager;

CTFCSTeamManager *TFCSTeamMgr()
{
	return &s_TFCSTeamManager;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCSTeamManager::CTFCSTeamManager()
{
	m_UndefinedTeamColor.r = 255;
	m_UndefinedTeamColor.g = 255;
	m_UndefinedTeamColor.b = 255;
	m_UndefinedTeamColor.a = 0;

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSTeamManager::Init( void )
{
	// Clear the list.
	Shutdown();

	// Create the team list.
	for ( int iTeam = 0; iTeam < TEAM_COUNT; ++iTeam )
	{
		int index = Create( g_aTeamNames[iTeam], g_aTeamColors[iTeam] );
		Assert( index == iTeam );
		if ( index != iTeam )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSTeamManager::Shutdown( void )
{
	// Note, don't delete each team since they are in the gEntList and will 
	// automatically be deleted from there, instead.
	g_Teams.Purge();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFCSTeamManager::Create( const char *pName, color32 color )
{
	CTeam *pTeam = static_cast<CTeam*>( CreateEntityByName( "tfcs_team" ) );
	if ( pTeam )
	{
		// Add the team to the global list of teams.
		int iTeam = g_Teams.AddToTail( pTeam );

		// Initialize the team.
		pTeam->Init( pName, iTeam );
		pTeam->NetworkProp()->SetUpdateInterval( 0.75f );

		// Set the team color.
		CTFCSTeam *pTFCSTeam = static_cast<CTFCSTeam*>( pTeam );
		pTFCSTeam->SetColor( color );

		return iTeam;
	}

	// Error.
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSTeamManager::IsValidTeam( int iTeam )
{
	if ( ( iTeam >= 0 ) && ( iTeam < g_Teams.Count() ) )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int	CTFCSTeamManager::GetTeamCount( void )
{
	return g_Teams.Count();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCSTeam *CTFCSTeamManager::GetTeam( int iTeam )
{
	Assert( ( iTeam >= 0 ) && ( iTeam < g_Teams.Count() ) );
	if ( IsValidTeam( iTeam ) )
	{
		return static_cast<CTFCSTeam*>( g_Teams[iTeam] );
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCSTeam *CTFCSTeamManager::GetSpectatorTeam()
{
	return GetTeam( 0 );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
color32 CTFCSTeamManager::GetUndefinedTeamColor( void )
{
	return m_UndefinedTeamColor;
}

//=============================================================================
//
// TFCS Team Functions.
//

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFCSTeam::CTFCSTeam()
{
	m_TeamColor.r = 0;
	m_TeamColor.g = 0;
	m_TeamColor.b = 0;
	m_TeamColor.a = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSTeam::SetColor( color32 color )
{
	m_TeamColor = color;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
color32 CTFCSTeam::GetColor( void )
{
	return m_TeamColor;
}

//-----------------------------------------------------------------------------
// Purpose: Get a pointer to the specified TFCS team
//-----------------------------------------------------------------------------
CTFCSTeam *GetGlobalTFCSTeam( int iIndex )
{
	if ( iIndex < 0 || iIndex >= GetNumberOfTeams() )
		return NULL;

	return ( dynamic_cast< CTFCSTeam* >( g_Teams[iIndex] ) );
}