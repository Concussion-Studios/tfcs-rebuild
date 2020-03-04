#include "cbase.h"
#include "tfcs_weapon_base_melee.h"
#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"
#include "animation.h"

#if defined( CLIENT_DLL )
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
	#include "ndebugoverlay.h"
	#include "te_effect_dispatch.h"
	#include "ilagcompensationmanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponBaseMelee, DT_TFCSWeaponBaseMelee )

BEGIN_NETWORK_TABLE( CTFCSWeaponBaseMelee, DT_TFCSWeaponBaseMelee )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponBaseMelee )
END_PREDICTION_DATA()

static const Vector g_MeleeMins( -16, -16, -16 );
static const Vector g_MeleeMaxs( 16, 16, 16 );

CTFCSWeaponBaseMelee::CTFCSWeaponBaseMelee()
{
	m_bFiresUnderwater = true;
}

void CTFCSWeaponBaseMelee::Spawn( void )
{
	m_fMinRange1	= 0;
	m_fMinRange2	= 0;
	m_fMaxRange1	= 64;
	m_fMaxRange2	= 64;

	//Call base class first
	BaseClass::Spawn();
}

void CTFCSWeaponBaseMelee::Precache( void )
{
	//Call base class first
	BaseClass::Precache();
}

void CTFCSWeaponBaseMelee::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return;

	if ( ( pOwner->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) )
		PrimaryAttack();
	else if ( ( pOwner->m_nButtons & IN_ATTACK2 ) && ( m_flNextSecondaryAttack <= gpGlobals->curtime ) )
		SecondaryAttack();
	else 
	{
		WeaponIdle();
		return;
	}
}

void CTFCSWeaponBaseMelee::PrimaryAttack()
{
#ifndef CLIENT_DLL
	CTFCSPlayer *pPlayer = ToTFCSPlayer( GetPlayerOwner() );

	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( pPlayer, pPlayer->GetCurrentCommand() );
#endif

	Swing( false );

#ifndef CLIENT_DLL
	// Move other players back to history positions based on local player's lag
	lagcompensation->FinishLagCompensation( pPlayer );
#endif

}

void CTFCSWeaponBaseMelee::SecondaryAttack()
{
	Swing( true );
}

void CTFCSWeaponBaseMelee::Hit( trace_t &traceHit, Activity nHitActivity )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	
	//Do view kick
	AddViewKick();

	CBaseEntity	*pHitEntity = traceHit.m_pEnt;

	//Apply damage to a hit target
	if ( pHitEntity != NULL )
	{
		Vector hitDirection;
		pPlayer->EyeVectors( &hitDirection, NULL, NULL );
		VectorNormalize( hitDirection );

#ifndef CLIENT_DLL
		CTakeDamageInfo info( GetOwner(), GetOwner(), GetDamageForActivity( nHitActivity ), DMG_CLUB );

		CalculateMeleeDamageForce( &info, hitDirection, traceHit.endpos );

		pHitEntity->DispatchTraceAttack( info, hitDirection, &traceHit ); 
		ApplyMultiDamage();

		// Now hit all triggers along the ray that... 
		TraceAttackToTriggers( info, traceHit.startpos, traceHit.endpos, hitDirection );
#endif
		WeaponSound( MELEE_HIT );
	}

	// Apply an impact effect
	ImpactEffect( traceHit );
}

Activity CTFCSWeaponBaseMelee::ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner )
{
	int			i, j, k;
	float		distance;
	const float	*minmaxs[2] = {mins.Base(), maxs.Base()};
	trace_t		tmpTrace;
	Vector		vecHullEnd = hitTrace.endpos;
	Vector		vecEnd;

	distance = 1e6f;
	Vector vecSrc = hitTrace.startpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
	if ( tmpTrace.fraction == 1.0 )
	{
		for ( i = 0; i < 2; i++ )
		{
			for ( j = 0; j < 2; j++ )
			{
				for ( k = 0; k < 2; k++ )
				{
					vecEnd.x = vecHullEnd.x + minmaxs[i][0];
					vecEnd.y = vecHullEnd.y + minmaxs[j][1];
					vecEnd.z = vecHullEnd.z + minmaxs[k][2];

					UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
					if ( tmpTrace.fraction < 1.0 )
					{
						float thisDistance = (tmpTrace.endpos - vecSrc).Length();
						if ( thisDistance < distance )
						{
							hitTrace = tmpTrace;
							distance = thisDistance;
						}
					}
				}
			}
		}
	}
	else
		hitTrace = tmpTrace;

	return ACT_VM_HITCENTER;
}

bool CTFCSWeaponBaseMelee::ImpactWater( const Vector &start, const Vector &end )
{
	//FIXME: This doesn't handle the case of trying to splash while being underwater, but that's not going to look good
	//		 right now anyway...
	
	// We must start outside the water
	if ( UTIL_PointContents( start ) & (CONTENTS_WATER|CONTENTS_SLIME))
		return false;

	// We must end inside of water
	if ( !(UTIL_PointContents( end ) & (CONTENTS_WATER|CONTENTS_SLIME)))
		return false;

	trace_t	waterTrace;

	UTIL_TraceLine( start, end, (CONTENTS_WATER|CONTENTS_SLIME), GetOwner(), COLLISION_GROUP_NONE, &waterTrace );

	if ( waterTrace.fraction < 1.0f )
	{
#ifndef CLIENT_DLL
		CEffectData	data;

		data.m_fFlags  = 0;
		data.m_vOrigin = waterTrace.endpos;
		data.m_vNormal = waterTrace.plane.normal;
		data.m_flScale = 8.0f;

		// See if we hit slime
		if ( waterTrace.contents & CONTENTS_SLIME )
			data.m_fFlags |= FX_WATER_IN_SLIME;

		DispatchEffect( "watersplash", data );			
#endif
	}

	return true;
}

void CTFCSWeaponBaseMelee::ImpactEffect( trace_t &traceHit )
{
	// See if we hit water (we don't do the other impact effects in this case)
	if ( ImpactWater( traceHit.startpos, traceHit.endpos ) )
		return;

	//FIXME: need new decals
	UTIL_ImpactTrace( &traceHit, DMG_CLUB );
}

void CTFCSWeaponBaseMelee::Swing( int bIsSecondary )
{
	trace_t traceHit;

	// Try a ray
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( !pOwner )
		return;

	Vector swingStart = pOwner->Weapon_ShootPosition( );
	Vector forward;

	pOwner->EyeVectors( &forward, NULL, NULL );

	Vector swingEnd = swingStart + forward * GetRange();
	UTIL_TraceLine( swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit );
	Activity nHitActivity = ACT_VM_HITCENTER;

#ifndef CLIENT_DLL
	// Like bullets, bludgeon traces have to trace against triggers.
	CTakeDamageInfo triggerInfo( GetOwner(), GetOwner(), GetDamageForActivity( nHitActivity ), DMG_CLUB );
	TraceAttackToTriggers( triggerInfo, traceHit.startpos, traceHit.endpos, vec3_origin );
#endif

	if ( traceHit.fraction == 1.0 )
	{
		float MeleeHullRadius = 1.732f * 16;  // hull is +/- 16, so use cuberoot of 2 to determine how big the hull is from center to the corner point

		// Back off by hull "radius"
		swingEnd -= forward * MeleeHullRadius;

		UTIL_TraceHull( swingStart, swingEnd, g_MeleeMins, g_MeleeMaxs, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit );
		if ( traceHit.fraction < 1.0 && traceHit.m_pEnt )
		{
			Vector vecToTarget = traceHit.m_pEnt->GetAbsOrigin() - swingStart;
			VectorNormalize( vecToTarget );

			float dot = vecToTarget.Dot( forward );

			// YWB:  Make sure they are sort of facing the guy at least...
			if ( dot < 0.70721f )
				traceHit.fraction = 1.0f;	// Force amiss
			else
				nHitActivity = ChooseIntersectionPointAndActivity( traceHit, g_MeleeMins, g_MeleeMaxs, pOwner );
		}
	}

	WeaponSound( SINGLE );

	// -------------------------
	//	Miss
	// -------------------------
	if ( traceHit.fraction == 1.0f )
	{
		nHitActivity = bIsSecondary ? ACT_VM_MISSCENTER2 : ACT_VM_MISSCENTER;

		// We want to test the first swing again
		Vector testEnd = swingStart + forward * GetRange();
		
		// See if we happened to hit water
		ImpactWater( swingStart, testEnd );
	}
	else
		Hit( traceHit, nHitActivity );

	// Send the anim
	SendWeaponAnim( nHitActivity );

	//ToTFCSPlayer( pOwner )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	//Setup our next attack times
	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
}