//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tfc_team.h"
#include "entitylist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// Datatable
IMPLEMENT_SERVERCLASS_ST(CTFCTeam, DT_TFCTeam)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_manager, CTFCTeam );

//-----------------------------------------------------------------------------
// Purpose: Get a pointer to the specified TF team manager
//-----------------------------------------------------------------------------
CTFCTeam *GetGlobalTFCTeam( int iIndex )
{
	return (CTFCTeam*)GetGlobalTeam( iIndex );
}


//-----------------------------------------------------------------------------
// Purpose: Needed because this is an entity, but should never be used
//-----------------------------------------------------------------------------
void CTFCTeam::Init( const char *pName, int iNumber )
{
	BaseClass::Init( pName, iNumber );

	// Only detect changes every half-second.
	NetworkProp()->SetUpdateInterval( 0.75f );
}

void CTFCTeam::AddPlayerClass( const char *szClassName )
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

const CTFCPlayerClassInfo &CTFCTeam::GetPlayerClassInfo( int iPlayerClass ) const
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

bool CTFCTeam::IsClassOnTeam( const char *pszClassName, int &iClassNum ) const
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

void CTFCTeam::ResetScores( void )
{
	SetRoundsWon(0);
	SetScore(0);
}

// BLUE TEAM
//==================
class CTFCTeam_Blue : public CTFCTeam
{
	DECLARE_CLASS( CTFCTeam_Blue, CTFCTeam );
	DECLARE_SERVERCLASS();

	virtual void Init( const char *pName, int iNumber )
	{
		BaseClass::Init( pName, iNumber );

		int i = 0;
		while( pszPlayerClasses[i] != NULL )
		{
			AddPlayerClass( pszPlayerClasses[i] );
			i++;
		}
	}

	virtual const char *GetTeamName( void ) { return "#Teamname_Blue"; }
};

IMPLEMENT_SERVERCLASS_ST(CTFCTeam_Blue, DT_TFCTeam_Blue)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_blue, CTFCTeam_Blue );


// RED TEAM
//==================
class CTFCTeam_Red : public CTFCTeam
{
	DECLARE_CLASS( CTFCTeam_Red, CTFCTeam );
	DECLARE_SERVERCLASS();

	virtual void Init( const char *pName, int iNumber )
	{
		BaseClass::Init( pName, iNumber );

		int i = 0;
		while( pszPlayerClasses[i] != NULL )
		{
			AddPlayerClass( pszPlayerClasses[i] );
			i++;
		}
	}

	virtual const char *GetTeamName( void ) { return "#Teamname_Red"; }
};

IMPLEMENT_SERVERCLASS_ST(CTFCTeam_Red, DT_TFCTeam_Red)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_red, CTFCTeam_Red );

// GREEN TEAM
//==================
class CTFCTeam_Green : public CTFCTeam
{
	DECLARE_CLASS( CTFCTeam_Green, CTFCTeam );
	DECLARE_SERVERCLASS();

	virtual void Init( const char *pName, int iNumber )
	{
		BaseClass::Init( pName, iNumber );

		int i = 0;
		while( pszPlayerClasses[i] != NULL )
		{
			AddPlayerClass( pszPlayerClasses[i] );
			i++;
		}
	}

	virtual const char *GetTeamName( void ) { return "#Teamname_Green"; }
};

IMPLEMENT_SERVERCLASS_ST(CTFCTeam_Green, DT_TFCTeam_Green)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_green, CTFCTeam_Green );


// YELLOW TEAM
//==================
class CTFCTeam_Yellow : public CTFCTeam
{
	DECLARE_CLASS( CTFCTeam_Yellow, CTFCTeam );
	DECLARE_SERVERCLASS();

	virtual void Init( const char *pName, int iNumber )
	{
		BaseClass::Init( pName, iNumber );

		int i = 0;
		while( pszPlayerClasses[i] != NULL )
		{
			AddPlayerClass( pszPlayerClasses[i] );
			i++;
		}
	}

	virtual const char *GetTeamName( void ) { return "#Teamname_Yellow"; }
};

IMPLEMENT_SERVERCLASS_ST(CTFCTeam_Yellow, DT_TFCTeam_Yellow)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_yellow, CTFCTeam_Yellow );

// UNASSIGNED
//==================
class CTFCTeam_Unassigned : public CTFCTeam
{
	DECLARE_CLASS( CTFCTeam_Unassigned, CTFCTeam );
	DECLARE_SERVERCLASS();

	virtual void Init( const char *pName, int iNumber )
	{
		BaseClass::Init( pName, iNumber );

		/*int i = 0;
		while( pszPlayerClasses[i] != NULL )
		{
			AddPlayerClass( pszPlayerClasses[i] );
			i++;
		}*/
	}

	virtual const char *GetTeamName( void ) { return "#Teamname_Unassigned"; }
};

IMPLEMENT_SERVERCLASS_ST(CTFCTeam_Unassigned, DT_TFCTeam_Unassigned)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tfc_team_unassigned, CTFCTeam_Unassigned );

