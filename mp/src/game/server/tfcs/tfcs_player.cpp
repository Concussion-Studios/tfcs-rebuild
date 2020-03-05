#include "cbase.h"
#include "tfcs_player.h"
#include "tfcs_gamerules.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "team.h"

#define TFCS_PLAYER_MODEL "models/player/scout.mdl"

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

	CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name ) {}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nData ), 32 )
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( pPlayer->EyePosition() );

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.m_nData = nData;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Filters updates to a variable so that only non-local players see
// the changes.  This is so we can send a low-res origin to non-local players
// while sending a hi-res one to the local player.
// Input  : *pVarData -
//			*pOut -
//			objectID -
//-----------------------------------------------------------------------------

void* SendProxy_SendNonLocalDataTable( const SendProp *pProp, const void *pStruct, const void *pVarData, CSendProxyRecipients *pRecipients, int objectID )
{
	pRecipients->SetAllRecipients();
	pRecipients->ClearRecipient( objectID - 1 );
	return ( void * )pVarData;
}
REGISTER_SEND_PROXY_NON_MODIFIED_POINTER( SendProxy_SendNonLocalDataTable );

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //
BEGIN_DATADESC( CTFCSPlayer )
	DEFINE_FIELD( m_flArmorClass, FIELD_FLOAT ),
	DEFINE_FIELD( m_iArmor, FIELD_INTEGER ),
	DEFINE_FIELD( m_iMaxArmor, FIELD_FLOAT ),
	DEFINE_FIELD( m_flConcussTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_flCrippleTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_iCrippleLevel, FIELD_INTEGER ),
END_DATADESC()

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSLocalPlayerExclusive )
	SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropInt( SENDINFO( m_flArmorClass ) ),
	SendPropInt( SENDINFO( m_iArmor ), 8, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iMaxArmor ), 8, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flConcussTime ) ),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CTFCSPlayer, DT_TFCSPlayer )

	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11 ),

	SendPropInt( SENDINFO( m_iRealSequence ), 9 ),

	// Data that only gets sent to the local player.
	SendPropDataTable( SENDINFO_DT( m_Shared ), &REFERENCE_SEND_TABLE( DT_TFCSPlayerShared ) ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "tfcs_localdata", 0, &REFERENCE_SEND_TABLE( DT_TFCSLocalPlayerExclusive ), SendProxy_SendLocalDataTable ),
	SendPropDataTable( "tfcs_nonlocaldata", 0, &REFERENCE_SEND_TABLE( DT_TFCSNonLocalPlayerExclusive ), SendProxy_SendNonLocalDataTable ),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( player, CTFCSPlayer );
PRECACHE_REGISTER( player );

CTFCSPlayer::CTFCSPlayer()
{
	m_PlayerAnimState = CreatePlayerAnimState( this );
	UseClientSideAnimation();
	m_angEyeAngles.Init();

	m_lifeState = LIFE_DEAD; // Start "dead".

	m_Shared.Init( this );

	//SetViewOffset( TFC_PLAYER_VIEW_OFFSET );

	SetContextThink( &CTFCSPlayer::TFCSPlayerThink, gpGlobals->curtime, "TFCSPlayerThink" );
}

CTFCSPlayer::~CTFCSPlayer()
{
	m_PlayerAnimState->Release();
}

CTFCSPlayer *CTFCSPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CTFCSPlayer::s_PlayerEdict = ed;
	return ( CTFCSPlayer* )CreateEntityByName( className );
}

void CTFCSPlayer::TFCSPlayerThink()
{
	SetContextThink( &CTFCSPlayer::TFCSPlayerThink, gpGlobals->curtime, "TFCSPlayerThink" );
}

void CTFCSPlayer::Precache()
{
	BaseClass::Precache();
}

void CTFCSPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn();
}

void CTFCSPlayer::Spawn()
{
	BaseClass::Spawn();
}

void CTFCSPlayer::GiveDefaultItems()
{

}

void CTFCSPlayer::ForceRespawn()
{
	BaseClass::ForceRespawn();
}

void CTFCSPlayer::PreThink()
{
	BaseClass::PreThink();
}

void CTFCSPlayer::Think()
{
	BaseClass::Think();
}

void CTFCSPlayer::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );
	
	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
}

int CTFCSPlayer::OnTakeDamage( const CTakeDamageInfo &info )
{
	return BaseClass::OnTakeDamage( info );
}

int CTFCSPlayer::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{

	return BaseClass::OnTakeDamage_Alive( info );
}

void CTFCSPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	DoAnimationEvent( PLAYERANIMEVENT_DIE );

	BaseClass::Event_Killed( info );
}

void CTFCSPlayer::CommitSuicide( bool bExplode /* = false */, bool bForce /*= false*/ )
{
	//m_iSuicideCustomKillFlags = DMG_CUSTOM_SUICIDE;

	BaseClass::CommitSuicide( bExplode, bForce );
}

bool CTFCSPlayer::ClientCommand( const CCommand &args )
{
	return BaseClass::ClientCommand( args );
}

void CTFCSPlayer::ChangeTeam( int iTeamNum )
{
	BaseClass::ChangeTeam( iTeamNum );
}

int CTFCSPlayer::TakeHealth( float flHealth )
{
	return 0;
}

int CTFCSPlayer::TakeArmor( float flArmor )
{
	return 0;
}

void CTFCSPlayer::SetArmorClass( float flArmorClass )
{

}