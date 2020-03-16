//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//
#ifndef TFCS_TEAM_H
#define TFCS_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"
#include "team.h"

//=============================================================================
// TFCS Teams.
//
class CTFCSTeam : public CTeam
{
	DECLARE_CLASS( CTFCSTeam, CTeam );
	DECLARE_SERVERCLASS();

public:

	CTFCSTeam();

	// TFCS Teams.
	void SetColor( color32 color );
	color32 GetColor( void );

private:
	
	color32 m_TeamColor;
};

class CTFCSTeamManager
{
public:

	CTFCSTeamManager();

	// Creation/Destruction.
	bool Init( void );
	void Shutdown( void );

	bool IsValidTeam( int iTeam );
	int GetTeamCount( void );
	CTFCSTeam *GetTeam( int iTeam );
	CTFCSTeam *GetSpectatorTeam();

	color32 GetUndefinedTeamColor( void );

private:

	int Create( const char *pName, color32 color );

private:

	color32	m_UndefinedTeamColor;
};

extern CTFCSTeamManager *TFCSTeamMgr();
extern CTFCSTeam *GetGlobalTFCSTeam( int iIndex );

#endif // TFCS_TEAM_H
