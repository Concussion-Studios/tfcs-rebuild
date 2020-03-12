#include "cbase.h"
#include "tfcs_player_shared.h"
#include "tfcs_playeranimstate.h"

#ifdef CLIENT_DLL
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"

	void TE_PlayerAnimEvent( CBasePlayer* pPlayer, PlayerAnimEvent_t playerAnim, int nData );
#endif

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

void CTFCSPlayerShared::Init( OuterClass* pPlayer )
{
	m_pOuter = pPlayer;
}

CTFCSWeaponBase* CTFCSPlayerShared::GetActiveTFCSWeapon() const
{
	CBaseCombatWeapon *pWeapon = m_pOuter->GetActiveWeapon();
	if ( pWeapon )
	{
		Assert( dynamic_cast< CTFCSWeaponBase* >( pWeapon ) == static_cast< CTFCSWeaponBase* >( pWeapon ) );
		return static_cast< CTFCSWeaponBase* >( pWeapon );
	}
	else
		return NULL;
}

bool CTFCSPlayerShared::IsDucking( void ) const
{
	return ( m_pOuter->GetFlags() & FL_DUCKING ) ? true : false;
}

bool CTFCSPlayerShared::IsOnGround() const
{
	return ( m_pOuter->GetFlags() & FL_ONGROUND ) ? true : false;
}

bool CTFCSPlayerShared::IsOnGodMode() const
{
	return ( m_pOuter->GetFlags() & FL_GODMODE ) ? true : false;
}

int CTFCSPlayerShared::GetButtons()
{
	return m_pOuter->m_nButtons;
}

bool CTFCSPlayerShared::IsButtonPressing( int btn )
{
	return ( ( m_pOuter->m_nButtons & btn ) ) ? true : false;
}

bool CTFCSPlayerShared::IsButtonPressed( int btn )
{
	return ( ( m_pOuter->m_afButtonPressed & btn ) ) ? true : false;
}

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

ITFCSPlayerAnimState* CreatePlayerAnimState( CTFCSPlayer *pPlayer )
{
	CTFCSPlayerAnimState *pRet = new CTFCSPlayerAnimState();
	pRet->Init( pPlayer );
	return pRet;
}

void CTFCSPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	m_PlayerAnimState->DoAnimationEvent( event, nData );

#ifndef CLIENT_DLL
	TE_PlayerAnimEvent( this, event, nData );	// Send to any clients who can see this guy.
#endif
}

void CTFCSPlayer::SetAnimation( PLAYER_ANIM playerAnim )
{
#ifndef CLIENT_DLL
	if ( playerAnim == PLAYER_ATTACK1 )
		DoAnimationEvent( PLAYERANIMEVENT_FIRE_GUN );

	int animDesired;
	char szAnim[64];

	float speed;

	speed = GetAbsVelocity().Length2D();

	if ( GetFlags() & ( FL_FROZEN | FL_ATCONTROLS ) )
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	if ( playerAnim == PLAYER_ATTACK1 )
	{
		if ( speed > 0 )
			playerAnim = PLAYER_WALK;
		else
			playerAnim = PLAYER_IDLE;
	}

	Activity idealActivity = ACT_WALK;// TEMP!!!!!

	// This could stand to be redone. Why is playerAnim abstracted from activity? (sjb)
	if ( playerAnim == PLAYER_JUMP )
		idealActivity = ACT_HOP;
	else if ( playerAnim == PLAYER_SUPERJUMP )
		idealActivity = ACT_LEAP;
	else if ( playerAnim == PLAYER_DIE )
	{
		if ( m_lifeState == LIFE_ALIVE )
			idealActivity = ACT_DIERAGDOLL;
	}
	else if ( playerAnim == PLAYER_ATTACK1 )
	{
		if ( GetActivity() == ACT_HOVER	|| GetActivity() == ACT_SWIM || GetActivity() == ACT_HOP || GetActivity() == ACT_LEAP || GetActivity() == ACT_DIESIMPLE )
			idealActivity = GetActivity();
		else
			idealActivity = ACT_RANGE_ATTACK1;
	}
	else if ( playerAnim == PLAYER_IDLE || playerAnim == PLAYER_WALK )
	{
		if ( !( GetFlags() & FL_ONGROUND ) && ( GetActivity() == ACT_HOP || GetActivity() == ACT_LEAP ) )	// Still jumping
			idealActivity = GetActivity();
		else if ( GetWaterLevel() > 1 )
		{
			if ( speed == 0 )
				idealActivity = ACT_HOVER;
			else
				idealActivity = ACT_SWIM;
		}
		else if ( speed > 0 )
			idealActivity = ACT_WALK;
		else
			idealActivity = ACT_IDLE;
	}

	if ( idealActivity == ACT_RANGE_ATTACK1 )
	{
		if ( GetFlags() & FL_DUCKING )	// crouching
			Q_strncpy( szAnim, "crouch_shoot_", sizeof( szAnim ) );
		else
			Q_strncpy( szAnim, "ref_shoot_", sizeof( szAnim ) );

		Q_strncat( szAnim, m_szAnimExtension, sizeof( szAnim ), COPY_ALL_CHARACTERS );

		animDesired = LookupSequence( szAnim );
		if ( animDesired == -1 )
			animDesired = 0;

		if ( GetSequence() != animDesired || !SequenceLoops() )
			SetCycle( 0 );

		SetActivity( idealActivity );
		ResetSequence( animDesired );
	}
	else if ( idealActivity == ACT_IDLE )
	{
		if ( GetFlags() & FL_DUCKING )
			animDesired = LookupSequence( "crouch_idle" );
		else
			animDesired = LookupSequence( "look_idle" );

		if ( animDesired == -1 )
			animDesired = 0;

		SetActivity( ACT_IDLE );
	}
	else if ( idealActivity == ACT_WALK )
	{
		if ( GetFlags() & FL_DUCKING )
		{
			animDesired = SelectWeightedSequence( ACT_CROUCH );
			SetActivity( ACT_CROUCH );
		}
		else
		{
			animDesired = SelectWeightedSequence( ACT_RUN );
			SetActivity( ACT_RUN );
		}
		
	}
	else
	{
		if ( GetActivity() == idealActivity )
			return;

		SetActivity( idealActivity );

		animDesired = SelectWeightedSequence( GetActivity() );

		// Already using the desired animation?
		if ( GetSequence() == animDesired )
			return;

		m_iRealSequence = animDesired;
		ResetSequence( animDesired );
		SetCycle( 0 );
		return;
	}

	// Already using the desired animation?
	if ( GetSequence() == animDesired )
		return;

	m_iRealSequence = animDesired;

	// Reset to first frame of desired animation
	ResetSequence( animDesired );
	SetCycle( 0 );
#else
	return; // This is handle in the server
#endif
}