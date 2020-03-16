#include "cbase.h"
#include "c_tfcs_player.h"
#include "c_basetempentity.h"
//#include "multiplayer_animstate.h"
#include "view.h"

#include "tier0/memdbgon.h"

#ifdef CTFCSPlayer
	#undef CTFCSPlayer
#endif

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType )
	{
		// Create the effect.
		auto *pPlayer = ToTFCSPlayer( m_hPlayer.Get() );
		if ( pPlayer && !pPlayer->IsDormant() )
			pPlayer->DoAnimationEvent( (PlayerAnimEvent_t)m_iEvent.Get(), m_nData );
	}

public:
	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

// ------------------------------------------------------------------------------------------ //
// Data tables and prediction tables.
// ------------------------------------------------------------------------------------------ //
BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_iEvent ) ),
	RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_TFCSPlayer )
END_PREDICTION_DATA()

BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSLocalPlayerExclusive )
//RecvPropVector(RECVINFO())
	RecvPropFloat( RECVINFO( m_ArmorClass ) ),
	RecvPropInt( RECVINFO( m_MaxArmor ) ),
	RecvPropFloat(RECVINFO(m_flConcussTime)),
END_RECV_TABLE()

//BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
//END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_TFCSPlayer, DT_TFCSPlayer, CTFCSPlayer )
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
	RecvPropInt( RECVINFO( m_iRealSequence ) ),

	RecvPropDataTable( RECVINFO_DT( m_Shared ), 0, &REFERENCE_RECV_TABLE( DT_TFCSPlayerShared ) ),

	RecvPropDataTable( "tfcs_localdata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSLocalPlayerExclusive ) ),
	//RecvPropDataTable( "tfcs_nonlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSNonLocalPlayerExclusive ) ),
END_RECV_TABLE()

ITFCSPlayerAnimState* CreatePlayerAnimState( C_TFCSPlayer *pPlayer );

C_TFCSPlayer::C_TFCSPlayer() : m_iv_angEyeAngles( "C_TFCSPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this );
	m_angEyeAngles.Init();

	m_Shared.Init( this );

// cant interpolate ... buggy?  it keeps resetting the angle to 0,0,0
//	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	//m_fNextThinkPushAway = 0.0f;
}

C_TFCSPlayer::~C_TFCSPlayer()
{
	m_PlayerAnimState->Release();
}

C_TFCSPlayer* C_TFCSPlayer::GetLocalTFCSPlayer()
{
	return ToTFCSPlayer( C_BasePlayer::GetLocalPlayer() );
}

void C_TFCSPlayer::ClientThink()
{
	// Pass on through to the base class.
	BaseClass::ClientThink();

	UpdateIDTarget();
}

void C_TFCSPlayer::TeamChange( int iNewTeam )
{
	BaseClass::TeamChange( iNewTeam );

	// The team number hasn't been updated yet.
	int iOldTeam = GetTeamNumber();
	C_BaseEntity::ChangeTeam( iNewTeam );

	// Reset back to old team just in case something uses it.
	C_BaseEntity::ChangeTeam( iOldTeam );

	TeamChangeStatic( iNewTeam );
}

void C_TFCSPlayer::TeamChangeStatic( int iNewTeam )
{
	// It's possible to receive events from the server before our local player is created.
	// All crucial things that don't rely on local player
	// should be put here.
	const char *pTeamConfig = "exec team_red.cfg";

	switch ( iNewTeam )
	{
	case TEAM_RED:
		pTeamConfig = "exec team_red.cfg";
		break;
	case TEAM_BLUE:
		pTeamConfig = "exec team_blue.cfg";
		break;
	case TEAM_GREEN:
		pTeamConfig = "exec team_green.cfg";
		break;
	case TEAM_YELLOW:
		pTeamConfig = "exec team_yellow.cfg";
		break;
	}

	if ( !( iNewTeam == TEAM_UNASSIGNED || iNewTeam == TEAM_SPECTATOR ) )
		engine->ClientCmd_Unrestricted( pTeamConfig );

}

const QAngle& C_TFCSPlayer::GetRenderAngles()
{
	if ( IsRagdoll() )
		return vec3_angle;
	else
		return m_PlayerAnimState->GetRenderAngles();
}

const QAngle& C_TFCSPlayer::EyeAngles()
{
	if ( IsLocalPlayer() )
		return BaseClass::EyeAngles();
	else
		return m_angEyeAngles;
}

void C_TFCSPlayer::UpdateClientSideAnimation()
{
	// Update the animation data. It does the local check here so this works when using
	// a third-person camera (and we don't have valid player angles).
	if ( this == C_BasePlayer::GetLocalPlayer() )
		m_PlayerAnimState->Update( EyeAngles()[YAW], m_angEyeAngles[PITCH] );
	else
		m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

	BaseClass::UpdateClientSideAnimation();
}

// Purpose: Update this client's target entity
void C_TFCSPlayer::UpdateIDTarget()
{
	if ( !IsLocalPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show id's in any state but active.
	//if ( State_Get() != STATE_ACTIVE )
	//	return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10,   MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && (pEntity != this) )
			m_iIDEntIndex = pEntity->entindex();
	}
}