#include "cbase.h"
#include "tfcs_gamerules.h"
#include "tfcs_player_shared.h"
#include "tfcs_playeranimstate.h"
#include "takedamageinfo.h"

#ifdef CLIENT_DLL
	#include "c_tfcs_player.h"
	#include "prediction.h"
#else
	#include "tfcs_player.h"

	void TE_PlayerAnimEvent( CBasePlayer* pPlayer, PlayerAnimEvent_t playerAnim, int nData );
#endif

void SpawnBlood( Vector vecSpot, const Vector& vecDir, int bloodColor, float flDamage );

//=============================================================================
//
// Tables.
//

#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE( CTFCSPlayerShared, DT_TFCSPlayerSharedLocal )
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( CTFCSPlayerShared, DT_TFCSPlayerShared )
	// Local Data.
	RecvPropDataTable( "tfcs_sharedlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSPlayerSharedLocal ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA_NO_BASE( CTFCSPlayerShared )
END_PREDICTION_DATA()

#else
BEGIN_SEND_TABLE_NOBASE( CTFCSPlayerShared, DT_TFCSPlayerSharedLocal )
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayerShared, DT_TFCSPlayerShared )
	// Local Data.
	SendPropDataTable( "tfcs_sharedlocaldata", 0, &REFERENCE_SEND_TABLE( DT_TFCSPlayerSharedLocal ), SendProxy_SendLocalDataTable ),
END_SEND_TABLE()
#endif

// --------------------------------------------------------------------------------------------------- //
// CTFCSPlayerShared implementation.
// --------------------------------------------------------------------------------------------------- //
CTFCSPlayerShared::CTFCSPlayerShared()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSPlayerShared::Init( OuterClass* pPlayer )
{
	m_pOuter = pPlayer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
/*CTFCSWeaponBase* CTFCSPlayerShared::GetActiveTFCSWeapon() const
{
	CBaseCombatWeapon *pWeapon = m_pOuter->GetActiveWeapon();
	if ( pWeapon )
	{
		Assert( dynamic_cast< CTFCSWeaponBase* >( pWeapon ) == static_cast< CTFCSWeaponBase* >( pWeapon ) );
		return static_cast< CTFCSWeaponBase* >( pWeapon );
	}
	else
		return NULL;
}*/

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsDucking( void ) const
{
	return ( m_pOuter->GetFlags() & FL_DUCKING ) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsOnGround() const
{
	return ( m_pOuter->GetFlags() & FL_ONGROUND ) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsOnGodMode() const
{
	return ( m_pOuter->GetFlags() & FL_GODMODE ) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFCSPlayerShared::GetButtons()
{
	return m_pOuter->m_nButtons;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsButtonPressing( int btn )
{
	return ( ( m_pOuter->m_nButtons & btn ) ) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsButtonPressed( int btn )
{
	return ( ( m_pOuter->m_afButtonPressed & btn ) ) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFCSPlayerShared::IsButtonReleased( int btn )
{
	return ( ( m_pOuter->m_afButtonReleased & btn ) ) ? true : false;
}

// --------------------------------------------------------------------------------------------------- //
// CTFCSPlayer implementation.
// --------------------------------------------------------------------------------------------------- //
CTFCSWeaponBase *CTFCSPlayer::Weapon_OwnsThisID( int iWeaponID )
{
	for ( int i = 0;i < WeaponCount(); i++ ) 
	{
		CTFCSWeaponBase *pWpn = ( CTFCSWeaponBase *)GetWeapon( i );
		if ( pWpn == NULL )
			continue;

		if ( pWpn->GetWeaponID() == iWeaponID )
			return pWpn;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCSWeaponBase* CTFCSPlayer::GetActiveTFCSWeapon() const
{
	return dynamic_cast< CTFCSWeaponBase* >( GetActiveWeapon() );
}

//-----------------------------------------------------------------------------
// Consider the weapon's built-in accuracy, this character's proficiency with
// the weapon, and the status of the target. Use this information to determine
// how accurately to shoot at the target.
//-----------------------------------------------------------------------------
Vector CTFCSPlayer::GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget )
{
	if ( pWeapon )
		return pWeapon->GetBulletSpread( WEAPON_PROFICIENCY_PERFECT );
	
	return VECTOR_CONE_15DEGREES;
}

//-----------------------------------------------------------------------------
// Purpose: multiplayer does not do autoaiming.
//-----------------------------------------------------------------------------
Vector CTFCSPlayer::GetAutoaimVector( float flDelta )
{
	//No Autoaim
	Vector	forward;
	AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
	return	forward;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFCSPlayer::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	if ( TFCSGameRules()->IsTeamplay() )
	{
		if ( collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT || collisionGroup == COLLISION_GROUP_PROJECTILE )
		{
			switch( GetTeamNumber() )
			{
			case TEAM_BLUE:
				if ( !( contentsMask & CONTENTS_BLUETEAM ) )
					return false;
				break;

			case TEAM_RED:
				if ( !( contentsMask & CONTENTS_REDTEAM ) )
					return false;
				break;

			case TEAM_GREEN:
				if ( !( contentsMask & CONTENTS_GREENTEAM ) )
					return false;
				break;

			case TEAM_YELLOW:
				if ( !( contentsMask & CONTENTS_YELLOWTEAM ) )
					return false;
				break;
			}
		}
	}

	return BaseClass::ShouldCollide( collisionGroup, contentsMask );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCSPlayer::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator )
{
	Vector vecOrigin = ptr->endpos - vecDir * 4;

	float flDistance = 0.0f;
	
	if ( info.GetAttacker() )
		flDistance = ( ptr->endpos - info.GetAttacker()->GetAbsOrigin() ).Length();

	if ( m_takedamage )
	{
		AddMultiDamage( info, this );

		int blood = BloodColor();
		
		CBaseEntity *pAttacker = info.GetAttacker();

		if ( pAttacker )
		{
			if ( TFCSGameRules()->IsTeamplay() && pAttacker->InSameTeam( this ) == true )
				return;
		}

		if ( blood != DONT_BLEED )
		{
			SpawnBlood( vecOrigin, vecDir, blood, flDistance );// a little surface blood.
			TraceBleed( flDistance, vecDir, ptr, info.GetDamageType() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CStudioHdr *CTFCSPlayer::OnNewModel( void )
{
	CStudioHdr *pHdr = BaseClass::OnNewModel();
	if ( pHdr )
	{
#ifdef CLIENT_DLL
		InitializePoseParams();
#endif // CLIENT_DLL

		// Reset the players animation states, gestures
		if ( GetAnimState() )
			GetAnimState()->OnNewModel();
	}

	return pHdr;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCSPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
#ifdef CLIENT_DLL
	if ( IsLocalPlayer() && prediction->InPrediction() && !prediction->IsFirstTimePredicted() )
		return;

	MDLCACHE_CRITICAL_SECTION();
#endif // CLIENT_DLL

	if ( GetAnimState() )
		GetAnimState()->DoAnimationEvent( event, nData );

#ifdef GAME_DLL
	TE_PlayerAnimEvent( this, event, nData );	// Send to any clients who can see this guy.
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: Do nothing multiplayer_animstate takes care of animation.
// Input  : playerAnim - 
//-----------------------------------------------------------------------------
void CTFCSPlayer::SetAnimation( PLAYER_ANIM playerAnim )
{
	if ( playerAnim == PLAYER_WALK || playerAnim == PLAYER_IDLE ) 
		return;

	if ( playerAnim == PLAYER_RELOAD )
		DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
	else if ( playerAnim == PLAYER_JUMP )
		DoAnimationEvent( PLAYERANIMEVENT_JUMP );
	else
		Assert( !"CTFCSPlayer::SetAnimation OBSOLETE!" );
}