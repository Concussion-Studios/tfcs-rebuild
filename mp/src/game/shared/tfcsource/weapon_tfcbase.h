//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
 
#ifndef WEAPON_TFCBASE_H
#define WEAPON_TFCBASE_H
#ifdef _WIN32
#pragma once
#endif
 
#include "tfc_playeranimstate.h"
#include "tfc_weapon_parse.h"
 
#if defined( CLIENT_DLL )
	#define CWeaponTFCBase C_WeaponTFCBase
#endif
 
class CTFCPlayer;
 
//Tony; use the same name as the base context one.
#define TFC_HIDEWEAPON_THINK_CONTEXT	"BaseCombatWeapon_HideThink"

// These are the names of the ammo types that the weapon script files reference.
class CWeaponTFCBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponTFCBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CWeaponTFCBase();

#ifdef CLIENT_DLL
	virtual bool ShouldPredict();
	virtual void OnDataChanged( DataUpdateType_t type );
	virtual void AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float CalcViewmodelBob( void );
#endif

	virtual void Precache( void );
	virtual void WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f );
	virtual void SetWeaponVisible( bool visible );

	// All predicted weapons need to implement and return true
	virtual bool	IsPredicted() const { return true; }
	virtual TFCWeaponID GetWeaponID( void ) const { return WEAPON_NONE; }
 
	// Get TFC weapon specific weapon data.
	CTFCWeaponInfo const	&GetTFCWpnData() const;
	virtual int				GetDamageType(void) { return g_aWeaponDamageTypes[GetWeaponID()]; }
 
	// Get a pointer to the player that owns this weapon
	CTFCPlayer* GetPlayerOwner() const;
 
	// override to play custom empty sounds
	virtual bool PlayEmptySound();

	// Weapon Types - this should be done in other way.
	virtual bool IsPistolWeapon() { return GetTFCWpnData().m_bIsPistol; }
	virtual bool IsShotgunWeapon() { return GetTFCWpnData().m_bIsShotgun; }
	virtual bool IsSniperWeapon() { return GetTFCWpnData().m_bIsSniper; }
	virtual bool IsHeavyWeapon() { return GetTFCWpnData().m_bIsHeavy; }
	virtual bool IsGrenadeWeapon() { return GetTFCWpnData().m_bIsGrenade; }
	virtual bool IsLaserWeapon() { return GetTFCWpnData().m_bIsLaser; }
	virtual bool IsBiozardWeapon() { return GetTFCWpnData().m_bIsBiozard; }
	virtual bool IsToolWeapon() { return GetTFCWpnData().m_bIsTool; }
 
	//Tony; these five functions return the sequences the view model uses for a particular action. -- You can override any of these in a particular weapon if you want them to do
	//something different, ie: when a pistol is out of ammo, it would show a different sequence.
	virtual Activity	GetPrimaryAttackActivity( void )	{	return	ACT_VM_PRIMARYATTACK;	}
	virtual Activity	GetIdleActivity( void ) { return ACT_VM_IDLE; }
	virtual Activity	GetDeployActivity( void ) { return ACT_VM_DRAW; }
	virtual Activity	GetReloadActivity( void ) { return ACT_VM_RELOAD; }
	virtual Activity	GetHolsterActivity( void ) { return ACT_VM_HOLSTER; }

	virtual const char	*GetDeploySound( void ) { return "Default.WeaponDeployPrimary"; }

	virtual void	WeaponIdle( void );
	virtual bool	Reload( void );
	virtual bool	Deploy();
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	SendReloadEvents();

	virtual void FallInit( void );
#ifdef GAME_DLL
	virtual void FallThink( void );	// make the weapon fall to the ground after spawning
	virtual void DoMuzzleFlash( void );
#endif

	//Tony; added so we can have base functionality without implementing it into every weapon.
	virtual void ItemPostFrame();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual float GetAccuracyModifier( void );

	virtual bool HasScope( void ) { return GetTFCWpnData().m_bUseScope; }	// not all of our weapons have scopes (although some do)
	virtual bool UnscopeAfterShot( void ) { return GetTFCWpnData().m_bUnscopeAfterShot; } // by default, allow scoping while firing
	virtual bool ShouldDrawScope( void ) { return m_bIsScoped; }
	virtual bool IsScoped( void ) { return m_bIsScoped; }
	virtual bool CanScope( void );
	virtual void EnterScope( void );
	virtual void ExitScope( bool unhideWeapon = true );
	virtual float GetScopeFOV( void ) { return GetTFCWpnData().m_flScopeFov; }
	virtual bool ShouldDrawCrosshair( void ) { return GetTFCWpnData().m_bDrawCosshair; }	// disables drawing crosshairs

	virtual bool ShouldDrawMuzzleFlash( void ) { return GetTFCWpnData().m_bDrawMuzzleFlash; }	// by default, all of our weapons have muzzleflashes
 
	//Tony; default weapon spread, pretty accurate - accuracy systems would need to modify this
	virtual float GetWeaponSpread() { return GetTFCWpnData().m_flSpread; }
	float m_flPrevAnimTime;

	//Tony; by default, all weapons are automatic.
	//If you add code to switch firemodes, this function would need to be overridden to return the correct current mode.
	virtual int GetFireMode() const { return FM_AUTOMATIC; }
 
	virtual float GetFireRate( void ) { return GetTFCWpnData().m_flCycleTime; };
 
	//Tony; by default, burst fire weapons use a max of 3 shots (3 - 1)
	//weapons with more, ie: a 5 round burst, can override and determine which firemode it's in.
	virtual int MaxBurstShots() const { return 2; }
 
	virtual float GetWeaponFOV() { return GetTFCWpnData().m_flWeaponFOV; }

	virtual float GetAmmoToRemove( void ) { return GetTFCWpnData().m_iAmmoToRemove; };

#ifdef GAME_DLL
	virtual void SetWeaponModelIndex( const char *pName ) { m_iWorldModelIndex = modelinfo->GetModelIndex( pName ); }
	virtual	void Materialize( void );
	virtual	int	ObjectCaps( void );
#endif
 
	virtual bool CanWeaponBeDropped() const { return true; }

	virtual acttable_t *ActivityList( int &iActivityCount );

	static acttable_t m_acttableTwoHandsGuns[];
	static acttable_t m_acttableHandGun[];
	static acttable_t m_acttableNoReload[];
	static acttable_t m_acttableAC[];
	static acttable_t m_acttableSniper[];
	static acttable_t m_acttableMelee[];
	static acttable_t m_acttableRPG[];
	static acttable_t m_acttableGrenade[];
	static acttable_t m_acttableTool[];

	float  m_flNextResetCheckTime;

	Vector	GetOriginalSpawnOrigin( void ) { return m_vOriginalSpawnOrigin;	}
	QAngle	GetOriginalSpawnAngles( void ) { return m_vOriginalSpawnAngles;	}

private:
 
	CNetworkVar(float, m_flDecreaseShotsFired);
	CNetworkVar(bool, m_bIsScoped);

	Vector m_vOriginalSpawnOrigin;
	QAngle m_vOriginalSpawnAngles;
 
	CWeaponTFCBase( const CWeaponTFCBase & );
};
 
 
#endif // WEAPON_TFCBASE_H