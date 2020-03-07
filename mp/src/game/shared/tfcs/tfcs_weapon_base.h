//***************************************************************
//
//	TFCS Weapon Base Code
//
//	CTFCSWeaponBase
//	|
//	|--> CTFCSWeaponBaseMelee
//	|		|
//	|		|--> CTFCSWeaponCrowbar
//	|		|--> CTFCSWeaponMedikit
//	|		|--> CTFCSWeaponSpanner
//	|		|--> CTFCSWeaponUmbrella
//	|		|--> CTFCSWeaponKnife
//	|
//	|--> CTFCSWeaponBaseGrenade
//	|		|
//	|		|--> Rest of the throwable nades
//	|
//	|--> CTFWeaponBaseGun
//	|		|
//	|		|--> -CTFCSWeaponShotgun
//	|		|--> -CTFCSWeaponRailgun
//	|		|--> -CTFCSWeaponAutorifle
//	|		|--> -CTFCSWeaponTranq
//	|		|--> -CTFCSWeaponSuperShotgun
//	|		|--> -CTFCSWeaponSniperrifle
//	|		|--> -CTFCSWeaponFlamethrower
//	|		|--> -CTFCSWeaponNailgun
//	|		|--> -CTFCSWeaponSuperNailgun
//	|		|--> -CTFCSWeaponGrenadeLauncher
//	|		|--> -CTFCSWeaponRocketLauncher
//	|		|--> -CTFCSWeaponPipebombLauncher
//	|		|--> -CTFCSWeaponAssaultCannon
//	|		|--> -CTFCSWeaponIncindiaryCannon
//
//***************************************************************
#ifndef TFCS_WEAPON_BASE_H
#define TFCS_WEAPON_BASE_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_weapon_parse.h"
#include "tfcs_player_shared.h"

#ifdef CLIENT_DLL
	#define CTFCSPlayer C_TFCSPlayer
	#define CTFCSWeaponBase C_TFCSWeaponBase
#else
	class CTFCSPlayer;
#endif

class CTFCSWeaponBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CTFCSWeaponBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponBase();
	~CTFCSWeaponBase();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	virtual void Spawn( void );

	virtual bool IsPredicted() const { return true; }
	
	virtual int GetWeaponID() { Assert(0); return WEAPON_NONE; }

	CTFCSWeaponInfo const &GetTFCSWpnData() const;

	CBasePlayer* GetPlayerOwner() const;
	CTFCSPlayer* GetTFCSPlayerOwner() const;

	virtual void PrimaryAttack();

	//virtual void Drop( const Vector &vecVelocity ); //todo: make weapons drop on owner death
	//void DropThink( void );

	virtual float GetFOV() { return -1; }

private:

	CTFCSWeaponBase( const CTFCSWeaponBase & );

#ifdef CLIENT_DLL
	virtual bool ShouldPredict();
	virtual void OnDataChanged( DataUpdateType_t type );
#endif
};
#endif //TFCS_WEAPON_BASE_H