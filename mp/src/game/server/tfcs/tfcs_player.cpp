#include "cbase.h"
#include "tfcs_player.h"
#include "tfcs_shareddefs.h"
#include "multiplayer_animstate.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "team.h"

#define TFCS_PLAYER_MODEL "models/player/scout.mdl"

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
	DEFINE_THINKFUNC( Think ),

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
	m_Shared.Init( this );
}

CTFCSPlayer::~CTFCSPlayer()
{
	
}

void CTFCSPlayer::Precache()
{
	BaseClass::Precache();
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

void CTFCSPlayer::TFCSPlayerThink()
{
	BaseClass::Think();
}

void CTFCSPlayer::PostThink()
{
	BaseClass::PostThink();
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
	BaseClass::Event_Killed( info );
}

bool CTFCSPlayer::ClientCommand( const CCommand &args )
{
	return false;
}

void CTFCSPlayer::ChangeTeam( int iTeamNum )
{

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

CTFCSPlayer *CTFCSPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CTFCSPlayer::s_PlayerEdict = ed;
	return ( CTFCSPlayer* )CreateEntityByName( className );
}

