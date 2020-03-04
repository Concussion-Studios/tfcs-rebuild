#include "cbase.h"
#include "tfcs_projectile_nails.h"
#include "tfcs_gamerules.h"

#ifdef CLIENT_DLL
	#include "c_basetempentity.h"
	#include "c_te_legacytempents.h"
	#include "c_te_effect_dispatch.h"
	#include "input.h"
	#include "c_tfcs_player.h"
	#include "cliententitylist.h"
	#include "particles_new.h"
#else
	#include "effect_dispatch_data.h"
	#include "bone_setup.h"
#endif

LINK_ENTITY_TO_CLASS( tfcs_projectile_nail, CProjectile_Nail );
PRECACHE_REGISTER( tfcs_projectile_nail );

short g_sModelIndexNail;
void PrecacheNail( void *pUser ) { g_sModelIndexNail = modelinfo->GetModelIndex( NAIL_MODEL ); }

PRECACHE_REGISTER_FN( PrecacheNail );

CProjectile_Nail::CProjectile_Nail()
{
}

CProjectile_Nail::~CProjectile_Nail()
{
}

CProjectile_Nail *CProjectile_Nail::CreateNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical )
{
	CProjectile_Nail* pNail = static_cast< CProjectile_Nail* >( CTFCSBaseProjectile::Create( "tfcs_projectile_nail", vecOrigin, vecAngles, pOwner, CProjectile_Nail::GetInitialVelocity(), g_sModelIndexNail, NAIL_DISPATCH_EFFECT, pScorer, bCritical ) );
#ifdef GAME_DLL
	if ( pNail )
		pNail->SetDamage( 9 );
#endif // GAME_DLL

	return pNail;
}

CProjectile_Nail *CProjectile_Nail::CreateSuperNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pWeapon, bool bCritical )
{
	CProjectile_Nail *pSuperNail = CreateNail( vecOrigin, vecAngles, pOwner, pWeapon );
#ifdef GAME_DLL
	if ( pSuperNail )
		pSuperNail->SetDamage( 12 );
#endif // GAME_DLL

	return pSuperNail;
}

CProjectile_Nail *CProjectile_Nail::CreateTranqNail( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pWeapon, bool bCritical )
{
	CProjectile_Nail *pDart = CreateNail( vecOrigin, vecAngles, pOwner, pWeapon );
#ifdef GAME_DLL
	if ( pDart )
		pDart->SetDamage( 18 );
#endif // GAME_DLL

	return pDart;
}

#ifdef CLIENT_DLL
void ClientsideProjectileNailCallback( const CEffectData &data )
{
	// Get the Nail and add it to the client entity list, so we can attach a particle system to it.
	C_TFCSPlayer *pPlayer = dynamic_cast< C_TFCSPlayer* >( ClientEntityList().GetBaseEntityFromHandle( data.m_hEntity ) );
	if ( pPlayer )
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback( data, NAIL_GRAVITY );
		if ( pNail )
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
	}
}

DECLARE_CLIENT_EFFECT( NAIL_DISPATCH_EFFECT, ClientsideProjectileNailCallback );
#endif