//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef WEAPON_TFCBASE_SNIPER_H
#define WEAPON_TFCBASE_SNIPER_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
	#define CWeaponTFCBaseSniper C_WeaponTFCBaseSniper
#endif

#include "cbase.h"
#include "weapon_tfcbase.h"

class CWeaponTFCBaseSniper : public CWeaponTFCBase
{
	DECLARE_CLASS( CWeaponTFCBaseSniper, CWeaponTFCBase );

public:
	CWeaponTFCBaseSniper( void );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

public:
	virtual void PrimaryAttack( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone;

		if ( IsScoped() )
			cone = Vector( 0, 0, 0 );	// do not take bullet spread into account when scoped
		else
			cone = VECTOR_CONE_10DEGREES;	// unscoped snipers are not at all accurate

		return cone;
	}

private:
	CWeaponTFCBaseSniper( const CWeaponTFCBaseSniper & );
};

#endif	// WEAPON_TFCBASE_SNIPER_H
