//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC CPlayerResource
//
// $NoKeywords: $
//=============================================================================//
#ifndef TFC_PLAYER_RESOURCE_H
#define TFC_PLAYER_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

class CTFCPlayerResource : public CPlayerResource
{
	DECLARE_CLASS( CTFCPlayerResource, CPlayerResource );
	
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTFCPlayerResource();

	virtual void UpdatePlayerData( void );
	virtual void Spawn( void );

protected:
	CNetworkArray( int, m_iPlayerClass, MAX_PLAYERS+1 );
};

#endif // TFC_PLAYER_RESOURCE_H
