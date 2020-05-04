//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef WEAPON_TFCBASE_RPG_H
#define WEAPON_TFCBASE_RPG_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_tfcbase.h"
 
#if defined( CLIENT_DLL )
	#define CWeaponTFCBaseRPG C_WeaponTFCBaseRPG
#endif

//-----------------------------------------------------------------------------
// RPG
//-----------------------------------------------------------------------------
class CWeaponTFCBaseRPG : public CWeaponTFCBase
{
public:
	DECLARE_CLASS( CWeaponTFCBaseRPG, CWeaponTFCBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponTFCBaseRPG();

	virtual const char	*GetDeploySound( void ) { return "Deploy.WeaponRPG"; }
	virtual bool CanDrop( void ) { return false; }
	virtual void PrimaryAttack();
	virtual bool Reload();
	virtual void WeaponIdle();
	void DoFireEffects();
	void Precache( void );
	virtual	void FireRocket( void );

private:
	CWeaponTFCBaseRPG( const CWeaponTFCBaseRPG & );
};

#endif // WEAPON_TFCBASE_RPG_H