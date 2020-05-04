//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//

#ifndef TFC_TEAM_H
#define TFC_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"
#include "team.h"
#include "playerclass_info_parse.h"
#include "tfc_playerclass_info_parse.h"

typedef CUtlLinkedList< PLAYERCLASS_FILE_INFO_HANDLE, int > PlayerClassInfoList;

//-----------------------------------------------------------------------------
// Purpose: Team Manager
//-----------------------------------------------------------------------------
class CTFCTeam : public CTeam
{
	DECLARE_CLASS( CTFCTeam, CTeam );
	DECLARE_SERVERCLASS();

public:

	// Initialization
	virtual void Init( const char *pName, int iNumber );
	const unsigned char *GetEncryptionKey( void ) { return g_pGameRules->GetEncryptionKey(); }

	CTFCPlayerClassInfo const &GetPlayerClassInfo( int iPlayerClass ) const;

	virtual void AddPlayerClass( const char *pszClassName );

	bool IsClassOnTeam( const char *pszClassName, int &iClassNum ) const;
	int GetNumPlayerClasses( void ) { return m_hPlayerClassInfoHandles.Count(); }

	virtual const char *GetTeamName( void ) { return "#Teamname_Spectators"; }

	void ResetScores( void );

private:
	CUtlVector < PLAYERCLASS_FILE_INFO_HANDLE >		m_hPlayerClassInfoHandles;
};

extern CTFCTeam *GetGlobalTFCTeam( int iIndex );

#endif // TF_TEAM_H
