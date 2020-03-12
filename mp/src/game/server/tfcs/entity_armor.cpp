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
		//Engineers can convert armor to metal
		if ( pTFCSPlayer->m_Shared.GetClassIndex() == CLASS_ENGINEER )
		{
			int iCells = pTFCSPlayer->GiveAmmo( GetArmorMetal(), AMMO_CELLS, true );
			if ( iCells )
			{
				bSuccess = true;
			}
		}
		
		float flCurrentArmorClass = pTFCSPlayer->GetArmorClass();
		int iCurrentArmor = pTFCSPlayer->ArmorValue();
		//Attempt to give armor
		
		//Upgrading or downgrading armor does not increment the player's current armor points
		//Don't downgrade armor unless the amount of extra protection offered by the lower class armor exceeds the amount of protection offered by the player's current armor (taken from Quake 1)
		if ( GetArmorClass() > flCurrentArmorClass || flCurrentArmorClass * iCurrentArmor < GetArmorClass() * GetArmorCount() )
		{
			pTFCSPlayer->SetArmorValue( GetArmorCount() );
			pTFCSPlayer->SetArmorClass( GetArmorClass() );
			bSuccess = true;
		}
		else
		{
			int iArmorGiven = pTFCSPlayer->TakeArmor( GetArmorCount() );
			if ( iArmorGiven > 0 )
			{
				bSuccess = true;
			}
		}

		//Play sound
		if ( bSuccess )
		{
			CPASAttenuationFilter filter( this, ARMOR_PICKUP_SOUND );
			EmitSound( filter, entindex(), ARMOR_PICKUP_SOUND );
		}
	}

	return bSuccess;
}