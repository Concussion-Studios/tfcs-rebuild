//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "tfc_gamerules.h"
#include "weapon_tfcbase_rpg.h"

#ifdef CLIENT_DLL
	#include "c_tfc_player.h"
	#include "prediction.h"
#else
	#include "tfc_player.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponTFCBaseRPG, DT_WeaponTFCBaseRPG )

BEGIN_NETWORK_TABLE( CWeaponTFCBaseRPG, DT_WeaponTFCBaseRPG )

END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponTFCBaseRPG )
END_PREDICTION_DATA()

CWeaponTFCBaseRPG::CWeaponTFCBaseRPG()
{
}

void CWeaponTFCBaseRPG::Precache()
{
	BaseClass::Precache();
}

bool CWeaponTFCBaseRPG::Reload( void )
{
	CTFCPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return false;

	Activity actReload;

	actReload = ACT_VM_RELOAD;

	int iResult = DefaultReload( GetMaxClip1(), GetMaxClip2(), actReload );
	if ( !iResult )
		return false;

	pPlayer->SetAnimation( PLAYER_RELOAD );

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();

	return true;
}

void CWeaponTFCBaseRPG::PrimaryAttack()
{
	CTFCPlayer *pPlayer = ToTFCPlayer( GetPlayerOwner() );
	
	// Out of ammo?
	if ( m_iClip1 <= 0 )
	{
		if ( m_bFireOnEmpty )
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
		}

		return;
	}
#ifdef CLIENT_DLL
	// Play Firing Sound
	WeaponSound(SINGLE);
#endif
	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		
	FireRocket();

	DoFireEffects();

	m_iClip1--; 

	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flTimeWeaponIdle = gpGlobals->curtime + GetFireRate();	//length of the fire anim!
}

void CWeaponTFCBaseRPG::FireRocket( void )
{
	Assert( !"Derived classes must implement this." );
}

void CWeaponTFCBaseRPG::DoFireEffects()
{
	CBasePlayer *pPlayer = GetPlayerOwner();
	if ( pPlayer )
		 pPlayer->DoMuzzleFlash();

	//smoke etc
}

void CWeaponTFCBaseRPG::WeaponIdle()
{
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	SendWeaponAnim( GetIdleActivity() );

	m_flTimeWeaponIdle = gpGlobals->curtime + SequenceDuration();
}