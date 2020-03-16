//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Client side C_TFCSTeam class
//
// $NoKeywords: $
//=============================================================================
#ifndef C_TFCS_TEAM_H
#define C_TFCS_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "c_team.h"
#include "shareddefs.h"
#include "c_baseobject.h"

class C_BaseEntity;

//-----------------------------------------------------------------------------
// Purpose: TFCS's Team manager
//-----------------------------------------------------------------------------
class C_TFCSTeam : public C_Team
{
	DECLARE_CLASS( C_TFCSTeam, C_Team );
	DECLARE_CLIENTCLASS();

public:

			C_TFCSTeam();
	virtual ~C_TFCSTeam();

	char *Get_Name( void );
};

C_TFCSTeam *GetGlobalTFCSTeam( int iTeamNumber );

#endif // C_TFCS_TEAM_H
