#include "cbase.h"
#include "tfcs_player_shared.h"

#ifdef CLIENT_DLL
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
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