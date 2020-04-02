//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ====
//
// Purpose: Used to relay outputs/inputs from the player to the world and vice versa
//
//=============================================================================

#include "cbase.h"
#include "logic_playerproxy.h"
#include "filters.h"
#include "hl2_player.h"

LINK_ENTITY_TO_CLASS( logic_playerproxy, CLogicPlayerProxy );

BEGIN_DATADESC( CLogicPlayerProxy )
	// Base
	DEFINE_OUTPUT( m_RequestedPlayerHealth,		"PlayerHealth" ),
	DEFINE_OUTPUT( m_PlayerDied,				"PlayerDied" ),
	DEFINE_FIELD(  m_hPlayer,					FIELD_EHANDLE ),

	// HL2 / Episodic
	DEFINE_OUTPUT( m_OnFlashlightOn,			"OnFlashlightOn" ),
	DEFINE_OUTPUT( m_OnFlashlightOff,			"OnFlashlightOff" ),
	DEFINE_OUTPUT( m_PlayerHasAmmo,				"PlayerHasAmmo" ),
	DEFINE_OUTPUT( m_PlayerHasNoAmmo,			"PlayerHasNoAmmo" ),
	DEFINE_INPUTFUNC( FIELD_VOID,				"RequestPlayerHealth",				InputRequestPlayerHealth ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,			"SetPlayerHealth",					InputSetPlayerHealth ),
	DEFINE_INPUTFUNC( FIELD_VOID,				"RequestAmmoState",					InputRequestAmmoState ),
END_DATADESC()

void CLogicPlayerProxy::Activate( void )
{
	BaseClass::Activate();

	if ( m_hPlayer == NULL )
		m_hPlayer = AI_GetSinglePlayer();
}

bool CLogicPlayerProxy::PassesDamageFilter( const CTakeDamageInfo &info )
{
	if ( m_hDamageFilter )
	{
		CBaseFilter *pFilter = (CBaseFilter *)( m_hDamageFilter.Get() );
		return pFilter->PassesDamageFilter( info );
	}

	return true;
}

void CLogicPlayerProxy::InputSetPlayerHealth( inputdata_t &inputdata )
{
	if ( m_hPlayer == NULL )
		return;

	m_hPlayer->SetHealth( inputdata.value.Int() );

}

void CLogicPlayerProxy::InputRequestPlayerHealth( inputdata_t &inputdata )
{
	if ( m_hPlayer == NULL )
		return;

	m_RequestedPlayerHealth.Set( m_hPlayer->GetHealth(), inputdata.pActivator, inputdata.pCaller );
}

void CLogicPlayerProxy::InputRequestAmmoState( inputdata_t &inputdata )
{
	if( m_hPlayer == NULL )
		return;

	CBasePlayer *pPlayer = ToBasePlayer( m_hPlayer );
	for ( int i = 0 ; i < pPlayer->WeaponCount(); ++i )
	{
		CBaseCombatWeapon* pCheck = pPlayer->GetWeapon( i );
		if ( pCheck )
		{
			if ( pCheck->HasAnyAmmo() && ( pCheck->UsesPrimaryAmmo() || pCheck->UsesSecondaryAmmo() ) )
			{
				m_PlayerHasAmmo.FireOutput( this, this, 0 );
				return;
			}
		}
	}

	m_PlayerHasNoAmmo.FireOutput( this, this, 0 );
}