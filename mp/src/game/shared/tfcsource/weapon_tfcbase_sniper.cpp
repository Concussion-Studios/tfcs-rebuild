//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#include "cbase.h"
#include "weapon_tfcbase_sniper.h"

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponTFCBaseSniper, DT_WeaponTFCBaseSniper )

BEGIN_NETWORK_TABLE( CWeaponTFCBaseSniper, DT_WeaponTFCBaseSniper )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponTFCBaseSniper )
END_PREDICTION_DATA()

CWeaponTFCBaseSniper::CWeaponTFCBaseSniper()
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = false;
}

void CWeaponTFCBaseSniper::PrimaryAttack( void )
{
	BaseClass::PrimaryAttack();

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}