#ifndef TFCS_PROJECTILE_NAIL_H
#define TFCS_PROJECTILE_NAIL_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "tfcs_projectile_base.h"

#ifdef CLIENT_DLL
	#define CProjectile_Nail C_Projectile_Nail
#endif

#define NAIL_MODEL "models/weapons/w_models/w_nail.mdl"
#define NAIL_DISPATCH_EFFECT "ClientProjectile_Nail"
#define NAIL_GRAVITY 0.3f

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CProjectile_Nail : public CTFCSBaseProjectile
{
	DECLARE_CLASS( CProjectile_Nail, CTFCSBaseProjectile );

public:

	CProjectile_Nail();
	~CProjectile_Nail();

	// Functions to create all the various types of nails.
	static CProjectile_Nail *CreateNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pWeapon = NULL, bool bCritical = false );
	static CProjectile_Nail *CreateSuperNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pWeapon = NULL, bool bCritical = false );
	static CProjectile_Nail *CreateTranqNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pWeapon = NULL, bool bCritical = false );

	virtual const char *GetProjectileModelName( void )	{ return NAIL_MODEL; }
	virtual float GetGravity( void ) { return NAIL_GRAVITY; }
	static float GetInitialVelocity( void ) { return 1500.0; }
};

#endif	// TFCS_PROJECTILE_NAIL_H