#ifndef HL2_PLAYER_H
#define HL2_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "logic_playerproxy.h"
#include "basemultiplayerplayer.h"

// Or pickup limits.
#define PLAYER_MAX_LIFT_MASS 85
#define PLAYER_MAX_LIFT_SIZE 128

class CHL2_Player : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS( CHL2_Player, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CHL2_Player();

	virtual void Spawn();
	virtual void UpdateClientData( void );
	virtual void Splash( void );

	void FlashlightTurnOn( void );
	void FlashlightTurnOff( void );

	void	FirePlayerProxyOutput( const char *pszOutputName, variant_t variant, CBaseEntity *pActivator, CBaseEntity *pCaller );
	CLogicPlayerProxy	*GetPlayerProxy( void );

	// Use + Pickup
	virtual void PlayerUse( void );
	virtual void PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize );
	virtual void ForceDropOfCarriedPhysObjects( CBaseEntity *pOnlyIfHoldingThis );
	virtual void ClearUsePickup();
	virtual bool CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit );
	CNetworkVar( bool, m_bPlayerPickedUpObject );
	bool PlayerHasObject() { return m_bPlayerPickedUpObject; }

	// Damage
	virtual bool PassesDamageFilter( const CTakeDamageInfo &info );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual int OnTakeDamage( const CTakeDamageInfo &info );
	virtual int OnTakeDamage_Alive( const CTakeDamageInfo &info );
	virtual void OnDamagedByExplosion( const CTakeDamageInfo &info );

	Class_T Classify ( void );

private:
	float m_flTimeUseSuspended;
	EHANDLE m_hPlayerProxy;	// Handle to a player proxy entity for quicker reference
};

#endif // HL2_PLAYER