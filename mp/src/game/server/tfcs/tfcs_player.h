#ifndef TFCS_PLAYER_H
#define TFCS_PLAYER_H
#pragma once

#include "basemultiplayerplayer.h"
#include "baseanimatingoverlay.h"
#include "basetempentity.h"
#include "tfcs_player_shared.h"
#include "tfcs_playeranimstate.h"
#include "tfcs_weapon_base.h"

class CTFCSPlayer;

//=============================================================================
//
// TE PlayerAnimEvent
//
class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

	CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name ) {}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

//=============================================================================
//
// TFCS Ragdoll
//
class CTFCSRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CTFCSRagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState() { return SetTransmitState( FL_EDICT_ALWAYS ); }

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
	CNetworkVar( bool, m_bGib );
	CNetworkVar( bool, m_bBurning );
};

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
	virtual void UpdateOnRemove();
	virtual void Think();
	virtual void PreThink();
	virtual void PostThink();
	virtual void CommitSuicide( bool bExplode = false, bool bForce = false );
	virtual int OnTakeDamage( const CTakeDamageInfo &info );
	virtual int OnTakeDamage_Alive( const CTakeDamageInfo &info );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual bool ClientCommand( const CCommand &args );
	virtual void ChangeTeam( int iTeamNum );
	bool HandleCommand_JoinClass( int iClass );
	void GiveDefaultItems();
	virtual CBaseEntity* EntSelectSpawnPoint( void );
	bool SelectSpawnSpot( const char* pEntClassName, CBaseEntity*& pSpot );
	virtual ITFCSPlayerAnimState* GetAnimState() { return m_PlayerAnimState; }
	virtual void CreateViewModel( int index );
	void NoteWeaponFired( void );
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;
	virtual void FireBullets( const FireBulletsInfo_t& info );
	virtual bool Event_Gibbed( const CTakeDamageInfo& info ) { return false; }
	virtual void DeathSound( const CTakeDamageInfo &info );

	// Ragdolls.
	virtual bool BecomeRagdollOnClient( const Vector& force );
	virtual void CreateRagdollEntity( void );
	void CreateRagdollEntity( bool bGib, bool bBurning );
	virtual void RemoveRagdollEntity();
	virtual bool ShouldGib( const CTakeDamageInfo& info );

	// Player avoidance
	void TFCSPushawayThink( void );

	//TFCS Functions
	void InitClass( void );
	void TFCSPlayerThink();
	virtual int TakeHealth( float flHealth );
	virtual int GiveAmmo( int iAmmo, int iAmmoIndex, bool bSuppressSound = false );
	int TakeArmor( int iArmor );
	float GetArmorClass( void ) { return m_ArmorClass; }
	void SetArmorClass( float flClass );
	int GetMaxArmor( void );
	void Concuss();
	void Cripple( int iCrippleLevel );
	int GetCrippleLevel();
	void SaveMe();
	void Burn();
	void Infect();
	void Heal( void );		//medic healing

public: // called by shared code

	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual void TraceAttack( const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator );
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;

	CNetworkVarEmbedded( CTFCSPlayerShared, m_Shared );

	CTFCSWeaponBase* Weapon_OwnsThisID( int iWeaponID );
	CTFCSWeaponBase* GetActiveTFCSWeapon() const;

	//TFCS related vars
	CNetworkVar( float, m_ArmorClass );
	CNetworkVarForDerived( int, m_ArmorValue );
	CNetworkVar( int, m_MaxArmor );
	CNetworkVar( float, m_flConcussTime );
	CNetworkVar( float, m_flCrippleTime );
	CNetworkVar( int, m_iCrippleLevel );

private:

	EHANDLE m_hBurnAttacker;	//For handling afterburn
	EHANDLE m_hInfecAttacker;
	EHANDLE m_hHealer;		//store healers for assists

	//TODO: Maybe something to buff afterburn damage if pyros hit an incindiary, a napalm, and flamethrower the player?

	ITFCSPlayerAnimState* m_PlayerAnimState;
	CNetworkQAngle( m_angEyeAngles );
	CNetworkVar( bool, m_bSpawnInterpCounter );
	CNetworkHandle( CBaseEntity, m_hRagdoll );
	CNetworkVar( int, m_iRealSequence );

	int m_iLastWeaponFireUsercmd;
	Vector m_vecTotalBulletForce;	//Accumulator for bullet force in a single frame

	CNetworkVar( int, m_cycleLatch ); // Network the cycle to clients periodically
	CountdownTimer m_cycleLatchTimer;

	friend class CTFCSRagdoll;
	friend class CTFCSPlayerAnimState;
};

inline CTFCSPlayer* ToTFCSPlayer( CBaseEntity* pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return nullptr;

	return static_cast<CTFCSPlayer*>( pEntity );
}

inline CTFCSPlayer* ToTFCSPlayer( CBasePlayer* pPlayer )
{
	return static_cast<CTFCSPlayer*>( pPlayer );
}

//TODO add playerclassinfo

#endif //TFCS_PLAYER_H