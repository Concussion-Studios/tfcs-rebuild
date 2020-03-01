#ifndef TFCS_PLAYER_SHARED_H
#define TFCS_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"
#include "tfcs_weapon_base.h"

#ifdef CLIENT_DLL
	class C_TFCSPlayer;
#else
	class CTFCSPlayer;
#endif

//=============================================================================
//
// Tables.
//
#ifdef CLIENT_DLL
	EXTERN_RECV_TABLE( DT_TFCSPlayerShared );
#else
	EXTERN_SEND_TABLE( DT_TFCSPlayerShared );
#endif

//=============================================================================
//
// Shared player class.
//
class CTFCSPlayerShared
{
public:

#ifdef CLIENT_DLL
	friend class C_TFCSPlayer;
	typedef C_TFCSPlayer OuterClass;
	DECLARE_PREDICTABLE();
#else
	friend class CTFCSPlayer;
	typedef CTFCSPlayer OuterClass;
#endif
	
	DECLARE_EMBEDDED_NETWORKVAR()
	DECLARE_CLASS_NOBASE( CTFCSPlayerShared );

	// Initialization.
	CTFCSPlayerShared();

	virtual void Init( OuterClass *pOuter );

	CTFCSWeaponBase *GetActiveTFCSWeapon() const;

	// Utils.
	bool	IsDucking( void ) const; 
	bool	IsOnGround( void ) const;
	bool	IsOnGodMode() const;
	int		GetButtons();
	bool	IsButtonPressing( int btn );
	bool	IsButtonPressed( int btn );
	bool	IsButtonReleased( int btn );

private:

	OuterClass *m_pOuter;
};			   

#endif // TFCS_PLAYER_SHARED_H