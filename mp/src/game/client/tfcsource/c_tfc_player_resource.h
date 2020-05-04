//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC C_PlayerResource
//
// $NoKeywords: $
//=============================================================================//
#ifndef C_TFC_PLAYER_RESOURCE_H
#define C_TFC_PLAYER_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_playerresource.h"

class C_TFC_PlayerResource : public C_PlayerResource
{
	DECLARE_CLASS( C_TFC_PlayerResource, C_PlayerResource );
public:
	DECLARE_CLIENTCLASS();

			C_TFC_PlayerResource();
	virtual ~C_TFC_PlayerResource();

	int GetPlayerClass( int iIndex );
	
protected:

	int m_iPlayerClass[MAX_PLAYERS+1];

};
C_TFC_PlayerResource * TheGameResources( void );

#endif // C_TFC_PLAYERRESOURCE_H