//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side C_TFCTeam class
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "engine/IEngineSound.h"
#include "hud.h"
#include "recvproxy.h"
#include "c_tfc_team.h"
#include "c_tfc_player_resource.h"

#include <vgui/ILocalize.h>
#include <tier3/tier3.h>
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//Tony; undefine what I did in the header so everything from this point forward functions correctly.
#undef CTFCTeam

IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam, DT_TFCTeam, CTFCTeam)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Get a pointer to the specified TF team manager
//-----------------------------------------------------------------------------
C_TFCTeam *GetGlobalTFCTeam( int iIndex )
{
	return (C_TFCTeam*)GetGlobalTeam( iIndex );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCTeam::C_TFCTeam()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFCTeam::~C_TFCTeam()
{
}

char *C_TFCTeam::Get_Name( void )
{
	wchar_t *teamname;
	if (m_szTeamname[0] == '#')
	{
		teamname = g_pVGuiLocalize->Find(m_szTeamname);

		char ansi[128];
		g_pVGuiLocalize->ConvertUnicodeToANSI( teamname, ansi, sizeof( ansi ) );

		return strdup(ansi);
	}
	else 
		return m_szTeamname;
}

void C_TFCTeam::AddPlayerClass( const char *szClassName )
{
	PLAYERCLASS_FILE_INFO_HANDLE hPlayerClassInfo;

	if ( ReadPlayerClassDataFromFileForSlot( filesystem, szClassName, &hPlayerClassInfo, GetEncryptionKey() ) )
	{
		m_hPlayerClassInfoHandles.AddToTail( hPlayerClassInfo );
	}
	else
	{
		Assert( !"missing playerclass script file" );
		Msg( "Missing playerclass script file for class: %s\n", szClassName );
	}	
}

const CTFCPlayerClassInfo &C_TFCTeam::GetPlayerClassInfo( int iPlayerClass ) const
{
	Assert( iPlayerClass >= 0 && iPlayerClass < m_hPlayerClassInfoHandles.Count() );

	const FilePlayerClassInfo_t *pPlayerClassInfo = GetFilePlayerClassInfoFromHandle( m_hPlayerClassInfoHandles[iPlayerClass] );
	const CTFCPlayerClassInfo *pTFCInfo;

#ifdef _DEBUG
	pTFCInfo = dynamic_cast< const CTFCPlayerClassInfo* >( pPlayerClassInfo );
	Assert( pTFCInfo );
#else
	pTFCInfo = static_cast< const CTFCPlayerClassInfo* >( pPlayerClassInfo );
#endif

	return *pTFCInfo;
}

bool C_TFCTeam::IsClassOnTeam( const char *pszClassName, int &iClassNum ) const
{
	iClassNum = PLAYERCLASS_UNDEFINED;

	// Random is always on every team
	if( FStrEq( pszClassName, "cls_random" ) )
	{
		iClassNum = PLAYERCLASS_RANDOM;
		return true;
	}
	
	for( int i=0;i<m_hPlayerClassInfoHandles.Count(); i++ )
	{
		FilePlayerClassInfo_t *pPlayerClassInfo = GetFilePlayerClassInfoFromHandle( m_hPlayerClassInfoHandles[i] );

		if( stricmp( pszClassName, pPlayerClassInfo->m_szSelectCmd ) == 0 )
		{
			iClassNum = i;
			return true;
		}
	}

	return false;
}

bool C_TFCTeam::IsClassOnTeam( int iClassNum ) const
{
	return ( iClassNum >= 0 && iClassNum < m_hPlayerClassInfoHandles.Count() );
}

int C_TFCTeam::CountPlayersOfThisClass( int iPlayerClass )
{
	int count = 0;

	C_TFC_PlayerResource *tfc_PR = dynamic_cast<C_TFC_PlayerResource *>(g_PR);

	Assert( tfc_PR );

	for ( int i=0;i<Get_Number_Players();i++ )
	{
		if ( iPlayerClass == tfc_PR->GetPlayerClass(m_aPlayers[i]) )
			count++;
	}

	return count;
}


IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam_Unassigned, DT_TFCTeam_Unassigned, CTFCTeam_Unassigned)
END_RECV_TABLE()

C_TFCTeam_Unassigned::C_TFCTeam_Unassigned()
{
	//parse our classes
	/*int i = 0;
	while( pszPlayerClasses[i] != NULL )
	{
		AddPlayerClass( pszPlayerClasses[i] );
		i++;
	}*/
}


IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam_Blue, DT_TFCTeam_Blue, CTFCTeam_Blue)
END_RECV_TABLE()

C_TFCTeam_Blue::C_TFCTeam_Blue()
{
	//parse our classes
	int i = 0;
	while( pszPlayerClasses[i] != NULL )
	{
		AddPlayerClass( pszPlayerClasses[i] );
		i++;
	}	
}


IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam_Red, DT_TFCTeam_Red, CTFCTeam_Red)
END_RECV_TABLE()

C_TFCTeam_Red::C_TFCTeam_Red()
{
	//parse our classes
	int i = 0;
	while( pszPlayerClasses[i] != NULL )
	{
		AddPlayerClass( pszPlayerClasses[i] );
		i++;
	}	
}

IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam_Green, DT_TFCTeam_Green, CTFCTeam_Green)
END_RECV_TABLE()

C_TFCTeam_Green::C_TFCTeam_Green()
{
	//parse our classes
	int i = 0;
	while( pszPlayerClasses[i] != NULL )
	{
		AddPlayerClass( pszPlayerClasses[i] );
		i++;
	}	
}


IMPLEMENT_CLIENTCLASS_DT(C_TFCTeam_Yellow, DT_TFCTeam_Yellow, CTFCTeam_Yellow)
END_RECV_TABLE()

C_TFCTeam_Yellow::C_TFCTeam_Yellow()
{
	//parse our classes
	int i = 0;
	while( pszPlayerClasses[i] != NULL )
	{
		AddPlayerClass( pszPlayerClasses[i] );
		i++;
	}	
}
