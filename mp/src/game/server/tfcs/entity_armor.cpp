//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC Armor.
//
//=============================================================================//
#include "cbase.h"
#include "tfcs_gamerules.h"
#include "tfcs_player.h"
#include "engine/IEngineSound.h"
#include "entity_armor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( entity_armor1, CEntityArmor1 );
LINK_ENTITY_TO_CLASS( entity_armor2, CEntityArmor2 );
LINK_ENTITY_TO_CLASS( entity_armor3, CEntityArmor3 );

void CEntityArmor::Spawn( void )
{
	Precache();
	SetModel( ARMOR_MODEL );
	BaseClass::Spawn();
}

void CEntityArmor::Precache( void )
{
	PrecacheModel( ARMOR_MODEL );
	PrecacheScriptSound( ARMOR_PICKUP_SOUND );
}

bool CEntityArmor::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFCSPlayer *pTFCSPlayer = ToTFCSPlayer( pPlayer );
	if ( pTFCSPlayer )
	{
		//Attempt to give armor
		//TODO: Don't downgrade armor unless below a certian value
		int iArmorGiven = pTFCSPlayer->TakeArmor( GetArmorCount() );

		if ( iArmorGiven > 0 )
		{
			pTFCSPlayer->SetArmorClass( GetArmorClass() );

			CPASAttenuationFilter filter( this, ARMOR_PICKUP_SOUND );
			EmitSound( filter, entindex(), ARMOR_PICKUP_SOUND );

			bSuccess = true;
		}
	}

	return bSuccess;
}