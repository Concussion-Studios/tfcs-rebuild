//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side CTFTeam class
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_TFC_TEAM_H
#define C_TFC_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "c_team.h"
#include "shareddefs.h"
#include "tfc_playerclass_info_parse.h"

class C_BaseEntity;
class C_BaseObject;
class CBaseTechnology;

//Tony; so we can call this from shared code!
#define CTFCTeam C_TFCTeam

//-----------------------------------------------------------------------------
// Purpose: TF's Team manager
//-----------------------------------------------------------------------------
class C_TFCTeam : public C_Team
{
	DECLARE_CLASS( C_TFCTeam, C_Team );
	DECLARE_CLIENTCLASS();

public:

					C_TFCTeam();
	virtual			~C_TFCTeam();

	virtual char	*Get_Name( void );

	CTFCPlayerClassInfo const &GetPlayerClassInfo( int iPlayerClass ) const;
	const unsigned char *GetEncryptionKey( void ) { return g_pGameRules->GetEncryptionKey(); }

	virtual void AddPlayerClass( const char *pszClassName );

	bool IsClassOnTeam( const char *pszClassName, int &iClassNum ) const;
	bool IsClassOnTeam( int iClassNum ) const;
	int GetNumPlayerClasses( void ) { return m_hPlayerClassInfoHandles.Count(); }

	int CountPlayersOfThisClass( int iPlayerClass );

private:
	CUtlVector < PLAYERCLASS_FILE_INFO_HANDLE >		m_hPlayerClassInfoHandles;
};

class C_TFCTeam_Unassigned : public C_TFCTeam
{
	DECLARE_CLASS( C_TFCTeam_Unassigned, C_TFCTeam );
public:
	DECLARE_CLIENTCLASS();

				     C_TFCTeam_Unassigned();
	 virtual		~C_TFCTeam_Unassigned() {}
};

class C_TFCTeam_Blue : public C_TFCTeam
{
	DECLARE_CLASS( C_TFCTeam_Blue, C_TFCTeam );
public:
	DECLARE_CLIENTCLASS();

				     C_TFCTeam_Blue();
	 virtual		~C_TFCTeam_Blue() {}
};

class C_TFCTeam_Red : public C_TFCTeam
{
	DECLARE_CLASS( C_TFCTeam_Red, C_TFCTeam );
public:
	DECLARE_CLIENTCLASS();

					 C_TFCTeam_Red();
	virtual			~C_TFCTeam_Red() {}
};

class C_TFCTeam_Green : public C_TFCTeam
{
	DECLARE_CLASS( C_TFCTeam_Green, C_TFCTeam );
public:
	DECLARE_CLIENTCLASS();

				     C_TFCTeam_Green();
	 virtual		~C_TFCTeam_Green() {}
};

class C_TFCTeam_Yellow : public C_TFCTeam
{
	DECLARE_CLASS( C_TFCTeam_Yellow, C_TFCTeam );
public:
	DECLARE_CLIENTCLASS();

					 C_TFCTeam_Yellow();
	virtual			~C_TFCTeam_Yellow() {}
};

extern C_TFCTeam *GetGlobalTFCTeam( int iIndex );

#endif // C_TFC_TEAM_H
