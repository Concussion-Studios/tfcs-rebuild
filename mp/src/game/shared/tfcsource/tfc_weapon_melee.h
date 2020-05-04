//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: Base code for any melee based weapon
//
//=====================================================================================//

#ifndef TFC_WEAPON_MELEE_H
#define TFC_WEAPON_MELEE_H

#ifdef _WIN32
#pragma once
#endif


#if defined( CLIENT_DLL )
#define CWeaponTFCMelee C_WeaponTFCMelee
#endif

//=========================================================
// CBaseHLBludgeonWeapon 
//=========================================================
class CWeaponTFCMelee : public CWeaponTFCBase
{
	DECLARE_CLASS( CWeaponTFCMelee, CWeaponTFCBase );
public:
	CWeaponTFCMelee();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual	void	Spawn( void );
	virtual	void	Precache( void );

	virtual const char	*GetDeploySound( void ) { return "Default.WeaponDeployMelee"; }
	
	//Attack functions
	virtual	void	PrimaryAttack( void );
	virtual	void	SecondaryAttack( void ) {}
	
	virtual void	ItemPostFrame( void );
	bool 			DoSwingTrace( trace_t &traceHit );

	//Functions to select animation sequences 
	virtual Activity	GetPrimaryAttackActivity( void )	{ return ACT_VM_HITCENTER; }
	virtual Activity	GetSecondaryAttackActivity( void )	{ return ACT_VM_HITCENTER2; }

	virtual float	GetRange( void )								{ return GetTFCWpnData().m_flMeleeRange; }
	virtual	float	GetDamageForActivity( Activity hitActivity )	{ return GetTFCWpnData().m_iDamage;	}

	CWeaponTFCMelee( const CWeaponTFCMelee & );

protected:
	virtual	void	ImpactEffect( trace_t &trace );

private:
	bool			ImpactWater( const Vector &start, const Vector &end );
	void			Swing( int bIsSecondary );
	void			Hit( trace_t &traceHit, Activity nHitActivity );
	void 			ChooseIntersectionPoint( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CTFCPlayer *pOwner );
	Activity		ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CTFCPlayer *pOwner );
};


#endif // TFC_WEAPON_MELEE_H