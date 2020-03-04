//***************************************************************
//
// TFCS Projectile Base Code
//
// CTFCSBaseProjectile
//	|
//	|-	CTFCSProjectile_Nail
//	|-	CTFCSProjectile_Dart
//	|-  CTFCSBaseRocket
//			|
//			|- Soldier rocket
//			|- Pyro rocket
//
//***************************************************************
#ifndef TFCS_BASE_PROJECTILE_H
#define TFCS_BASE_PROJECTILE_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_baseanimating.h"
	#include "tempent.h"
#else
	#include "baseanimating.h"
	#include "iscorer.h"
#endif

#ifdef CLIENT_DLL
#define CTFCSBaseProjectile C_TFCSBaseProjectile
C_LocalTempEntity *ClientsideProjectileCallback( const CEffectData &data, float flGravityBase, const char *pszParticleName = NULL );
#endif

//=============================================================================
//
// Generic projectile
//
class CTFCSBaseProjectile : public CBaseAnimating
#if !defined( CLIENT_DLL )
	, public IScorer
#endif
{
public:

	DECLARE_CLASS( CTFCSBaseProjectile, CBaseAnimating );
	DECLARE_NETWORKCLASS();

	CTFCSBaseProjectile();
	~CTFCSBaseProjectile();

	void Precache( void );
	void Spawn( void );

	virtual int GetWeaponID( void ) const { return m_iWeaponID; }
	void SetWeaponID( int iID ) { m_iWeaponID = iID; }

private:

	int m_iWeaponID;

protected:

	// Networked.
	CNetworkVector( m_vInitialVelocity );

	static CTFCSBaseProjectile *Create( const char *pszClassname, const Vector &vecOrigin, 
		const QAngle &vecAngles, CBaseEntity *pOwner, float flVelocity, short iProjModelIndex, const char *pszDispatchEffect = NULL, CBaseEntity *pScorer = NULL, bool bCritical = false );

	virtual const char *GetProjectileModelName( void );
	virtual float GetGravity( void ) { return 0.001f; }

#ifdef CLIENT_DLL
public:

	virtual int DrawModel( int flags );
	virtual void PostDataUpdate( DataUpdateType_t type );

private:

	float	 m_flSpawnTime;
#else
public:

	DECLARE_DATADESC();

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }
	void SetScorer( CBaseEntity *pScorer );

	virtual void ProjectileTouch( CBaseEntity *pOther );

	virtual float GetDamage() { return m_flDamage; }
	virtual void SetDamage(float flDamage) { m_flDamage = flDamage; }

	virtual Vector GetDamageForce( void );
	virtual int GetDamageType( void );

	unsigned int PhysicsSolidMaskForEntity( void ) const;

	void SetupInitialTransmittedGrenadeVelocity( const Vector &velocity )	{ m_vInitialVelocity = velocity; }

protected:

	void FlyThink( void );

protected:
	float m_flDamage;

	CBaseHandle m_Scorer;

#endif // CLIENT_DLL
};

#endif	// TFCS_BASE_PROJECTILE_H