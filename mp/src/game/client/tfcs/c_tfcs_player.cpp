#include "cbase.h"
#include "c_tfcs_player.h"
#include "c_basetempentity.h"
#include "multiplayer_animstate.h"

#include "tier0/memdbgon.h"

#ifdef CTFCSPlayer
#undef CTFCSPlayer
#endif

BEGIN_PREDICTION_DATA( C_TFCSPlayer )
END_PREDICTION_DATA()

BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSLocalPlayerExclusive )
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_TFCSPlayer, DT_TFCSPlayer, CTFCSPlayer )
	RecvPropDataTable( RECVINFO_DT( m_Shared ), 0, &REFERENCE_RECV_TABLE( DT_TFCSPlayerShared ) ),

	RecvPropDataTable( "tfcs_localdata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSLocalPlayerExclusive ) ),
	RecvPropDataTable( "tfcs_nonlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSNonLocalPlayerExclusive ) ),
END_RECV_TABLE()

C_TFCSPlayer::C_TFCSPlayer() : m_iv_angEyeAngles( "C_TFCSPlayer::m_iv_angEyeAngles" )
{
	//m_PlayerAnimState = CreateTFCSPlayerAnimState( this );

	m_Shared.Init( this );

	m_angEyeAngles.Init();
	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	//m_fNextThinkPushAway = 0.0f;
}

C_TFCSPlayer::~C_TFCSPlayer()
{
	
}

C_TFCSPlayer* C_TFCSPlayer::GetLocalTFCSPlayer()
{
	return ToTFCSPlayer(C_BasePlayer::GetLocalPlayer());
}