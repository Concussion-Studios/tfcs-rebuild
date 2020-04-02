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

// These are the names of the ammo types that go in the CAmmoDefs and that the 
// weapon script files reference.

// Given an ammo type (like from a weapon's GetPrimaryAmmoType()), this compares it
// against the ammo name you specify.
// MIKETODO: this should use indexing instead of searching and strcmp()'ing all the time.
bool IsAmmoType( int iAmmoType, const char *pAmmoName );

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

	virtual void Materialize( void );
	virtual	int	ObjectCaps( void );
	virtual void FallThink( void );	// make the weapon fall to the ground after spawning
	virtual void FallInit( void );
#endif

	virtual void Spawn( void );
	virtual bool IsPredicted() const { return true; }
	virtual int GetWeaponID() { Assert(0); return WEAPON_NONE; }

	void WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f );
	CTFCSWeaponInfo const &GetTFCSWpnData() const;

	CBasePlayer* GetPlayerOwner() const;
	CTFCSPlayer* GetTFCSPlayerOwner() const;

	virtual void PrimaryAttack();

	//virtual void Drop( const Vector &vecVelocity ); //todo: make weapons drop on owner death
	//void DropThink( void );

	virtual float GetFOV() { return -1; }

	virtual bool Reload();

public:

#ifdef CLIENT_DLL
	virtual bool ShouldPredict();
	virtual void OnDataChanged( DataUpdateType_t type );
#endif

	float m_flPrevAnimTime;
	float  m_flNextResetCheckTime;

	Vector	GetOriginalSpawnOrigin( void ) { return m_vOriginalSpawnOrigin;	}
	QAngle	GetOriginalSpawnAngles( void ) { return m_vOriginalSpawnAngles;	}

private:

	CTFCSWeaponBase( const CTFCSWeaponBase & );

	Vector m_vOriginalSpawnOrigin;
	QAngle m_vOriginalSpawnAngles;
};
#endif //TFCS_WEAPON_BASE_H