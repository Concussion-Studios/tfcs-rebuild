#ifndef TFCS_PLAYER_H
#define TFCS_PLAYER_H
#pragma once

#include "basemultiplayerplayer.h"
#include "tfcs_player_shared.h"

class CTFCSPlayer;

//=============================================================================
//
// TFCS Player
//
class CTFCSPlayer : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS(CTFCSPlayer, CBaseMultiplayerPlayer);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTFCSPlayer();
	~CTFCSPlayer();

	static CTFCSPlayer	*CreatePlayer( const char *className, edict_t *ed );
	static CTFCSPlayer	*Instance( int iEnt );

	virtual void Precache();
	virtual void InitialSpawn( void );
	virtual void Spawn();
	virtual void ForceRespawn();
	virtual void Think();
	virtual void PreThink();
	virtual void PostThink();
	virtual void CommitSuicide( bool bExplode = false, bool bForce = false );
	virtual int OnTakeDamage( const CTakeDamageInfo &info );
	virtual int OnTakeDamage_Alive( const CTakeDamageInfo &info );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual bool ClientCommand( const CCommand &args );
	virtual void ChangeTeam( int iTeamNum );
	void GiveDefaultItems();

	//TFCS Functions
	void TFCSPlayerThink();
	virtual int TakeHealth( float flHealth );
	int TakeArmor( float flArmor );
	void SetArmorClass( float flClass );
	void Concuss();
	void Cripple( int iCrippleLevel );
	int GetCrippleLevel();
	void SaveMe();
	void Burn();
	void Infect();


public: // called by shared code

	//virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	CNetworkVarEmbedded( CTFCSPlayerShared, m_Shared );

private:

	//TFCS related vars
	CNetworkVar( float, m_flArmorClass );
	CNetworkVar( int, m_iArmor );
	CNetworkVar( int, m_iMaxArmor );
	CNetworkVar( float, m_flConcussTime );
	CNetworkVar( float, m_flCrippleTime );
	CNetworkVar( int, m_iCrippleLevel );

	EHANDLE m_hBurnAttacker;
	EHANDLE m_hInfecAttacker;

	CNetworkQAngle( m_angEyeAngles );
	CNetworkHandle( CBaseEntity, m_hRagdoll );
};

inline CTFCSPlayer *ToTFCSPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return static_cast<CTFCSPlayer*>( pEntity );
}

//TODO add playerclassinfo

#endif //TFCS_PLAYER_H