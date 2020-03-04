#ifndef TFCS_WEAPON_BASE_MELEE_H
#define TFCS_WEAPON_BASE_MELEE_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_weapon_base.h"

#if defined( CLIENT_DLL )
#define CTFCSWeaponBaseMelee C_TFCSWeaponBaseMelee
#endif

class CTFCSWeaponBaseMelee : public CTFCSWeaponBase
{
	DECLARE_CLASS( CTFCSWeaponBaseMelee, CTFCSWeaponBase );
public:
	CTFCSWeaponBaseMelee();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual	void Spawn( void );
	virtual	void Precache( void );
	
	//Attack functions
	virtual	void PrimaryAttack( void );
	virtual	void SecondaryAttack( void );
	
	virtual void ItemPostFrame( void );

	//Functions to select animation sequences 
	virtual Activity GetPrimaryAttackActivity( void ) {	return ACT_VM_HITCENTER; }
	virtual Activity GetSecondaryAttackActivity( void )	{ return ACT_VM_HITCENTER2; }

	virtual	float GetFireRate( void ) { return 0.2f; }
	virtual float GetRange( void ) { return 32.0f; }
	virtual	float GetDamageForActivity( Activity hitActivity ) { return 1.0f; }

	CTFCSWeaponBaseMelee( const CTFCSWeaponBaseMelee & );

protected:
	virtual	void ImpactEffect( trace_t &trace );

private:
	bool ImpactWater( const Vector &start, const Vector &end );
	void Swing( int bIsSecondary );
	void Hit( trace_t &traceHit, Activity nHitActivity );
	Activity ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner );
};
#endif // TFCS_WEAPON_BASE_MELEE_H