//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's custom C_PlayerResource
//
// $NoKeywords: $
//=============================================================================//
#ifndef C_TFCS_PLAYERRESOURCE_H
#define C_TFCS_PLAYERRESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_playerresource.h"

class C_TFCS_PlayerResource : public C_PlayerResource
{
	DECLARE_CLASS( C_TFCS_PlayerResource, C_PlayerResource );
public:
	DECLARE_CLIENTCLASS();

			C_TFCS_PlayerResource();
	virtual ~C_TFCS_PlayerResource();
};

#endif // C_TFCS_PLAYERRESOURCE_H