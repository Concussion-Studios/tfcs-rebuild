//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFCS's custom CPlayerResource
//
// $NoKeywords: $
//=============================================================================//
#ifndef TFCS_PLAYER_RESOURCE_H
#define TFCS_PLAYER_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

class CTFCSPlayerResource : public CPlayerResource
{
	DECLARE_CLASS( CTFCSPlayerResource, CPlayerResource );
	
public:
	DECLARE_SERVERCLASS();

	CTFCSPlayerResource();
};

#endif // TFCS_PLAYER_RESOURCE_H
