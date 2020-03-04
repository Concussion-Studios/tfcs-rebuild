#ifndef TFCS_WEAPON_BASE_GUN_H
#define TFCS_WEAPON_BASE_GUN_H
#ifdef WIN32
#pragma once
#endif

#include "tfcs_weapon_base.h"

class CTFCSWeaponBaseGun : CTFCSWeaponBase
{
	DECLARE_CLASS( CTFCSWeaponBaseGun, CTFCSWeaponBase );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponBaseGun();
	~CTFCSWeaponBaseGun();

	virtual void PrimaryAttack( void );
	virtual void SecondaryAttack( void );
	virtual bool StartReload( void );
	virtual bool Reload( void );
	virtual void FinishReload( void );

	void FireWeapon( void );
};
#endif //TFCS_WEAPON_BASE_GUN_H