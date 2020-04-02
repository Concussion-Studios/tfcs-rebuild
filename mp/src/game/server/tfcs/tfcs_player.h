//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef TFCS_PLAYER_H
#define TFCS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

class CTFCSPlayer;

#include "basetempentity.h"
#include "hl2_player.h"
#include "simtimer.h"
#include "soundenvelope.h"
#include "tfcs_playeranimstate.h"
#include "tfcs_player_shared.h"
#include "tfcs_gamerules.h"
#include "tfcs_weapon_base.h"
#include "utldict.h"

// -------------------------------------------------------------------------------- //
// Function table for each player state.
// -------------------------------------------------------------------------------- //
class CTFCSPlayerStateInfo
{
public:
	TFCSPlayerState m_iPlayerState;
	const char *m_pStateName;

	void ( CTFCSPlayer::*pfnEnterState )();	// Init and deinit the state.
	void ( CTFCSPlayer::*pfnLeaveState )();
	void ( CTFCSPlayer::*pfnPreThink )();	// Do a PreThink() in this state.
};

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //
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

// -------------------------------------------------------------------------------- //
// Ragdoll entities.
// -------------------------------------------------------------------------------- //
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

// -------------------------------------------------------------------------------- //
// >>> CTFCSPlayer
// -------------------------------------------------------------------------------- //
class CTFCSPlayer : public CHL2_Player
{
public:
	DECLARE_CLASS( CTFCSPlayer, CHL2_Player );

	CTFCSPlayer();
	~CTFCSPlayer( void );
	
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	DECLARE_PREDICTABLE();

	static CTFCSPlayer	*CreatePlayer( const char *className, edict_t *ed );
	static CTFCSPlayer	*Instance( int iEnt );

	// This passes the event to the client's and server's CTFCSPlayerAnimState.
	void SetupBones( matrix3x4_t *pBoneToWorld, int boneMask );

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void PostThink( void );
	virtual void PreThink( void );
	virtual bool HandleCommand_JoinTeam( int team );
	virtual bool ClientCommand( const CCommand &args );
	virtual void CreateViewModel( int viewmodelindex = 0 );
	virtual bool BecomeRagdollOnClient( const Vector &force ) {	return true; }
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual int OnTakeDamage( const CTakeDamageInfo &inputInfo );
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;
	virtual void FireBullets ( const FireBulletsInfo_t &info );
	virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0);
	virtual bool BumpWeapon( CBaseCombatWeapon *pWeapon );
	virtual void ChangeTeam( int iTeam );
	virtual void UpdateOnRemove( void );
	virtual void DeathSound( const CTakeDamageInfo &info );
	bool SelectSpawnSpot( const char* pEntClassName, CBaseEntity*& pSpot );
	virtual CBaseEntity* EntSelectSpawnPoint( void );
	virtual void InitialSpawn( void );
	virtual void ForceRespawn();
	virtual void Think();
	virtual void CommitSuicide( bool bExplode = false, bool bForce = false );
	virtual int OnTakeDamage_Alive( const CTakeDamageInfo &info );
	bool HandleCommand_JoinClass( int iClass );
	virtual bool Event_Gibbed( const CTakeDamageInfo& info ) { return false; }

	int FlashlightIsOn( void );
	void FlashlightTurnOn( void );
	void FlashlightTurnOff( void );

	void CheatImpulseCommands( int iImpulse );
	void GiveAllItems( void );
	void GiveDefaultItems( void );

	void NoteWeaponFired( void );
	
	float GetNextTeamChangeTime( void ) { return m_flNextTeamChangeTime; }
	void  PickDefaultSpawnTeam( void );

	// Ragdolls.
	virtual void CreateRagdollEntity( void );
	void CreateRagdollEntity( bool bGib, bool bBurning );
	virtual void RemoveRagdollEntity();
	virtual bool ShouldGib( const CTakeDamageInfo& info );

	Vector m_vecTotalBulletForce;	//Accumulator for bullet force in a single frame

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 

	// Player avoidance
	void TFCSPushawayThink(void);

	virtual CTFCSPlayerAnimState* GetAnimState() { return m_PlayerAnimState; }

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

	virtual Vector GetAutoaimVector( float flDelta );
	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual void TraceAttack( const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator );
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;
	virtual CStudioHdr *OnNewModel( void );		//Tony; when model is changed, need to init some stuff.

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

	//Player state stuff
public:
	void State_Enter( TFCSPlayerState newState );
	void State_Transition( TFCSPlayerState newState );
	void State_PreThink();
	void State_Leave();
	static CTFCSPlayerStateInfo* State_LookupInfo( TFCSPlayerState state );

	void State_Enter_WELCOME();
	void State_PreThink_WELCOME();

	void State_Enter_PICKINGTEAM();
	void State_Enter_PICKINGCLASS();

private:
	void State_Enter_ACTIVE();
	void State_PreThink_ACTIVE();

	void State_Enter_OBSERVER_MODE();
	void State_PreThink_OBSERVER_MODE();

	void State_Enter_DEATH_ANIM();
	void State_PreThink_DEATH_ANIM();

	void ShowClassSelectMenu();

	void PhysObjectSleep();
	void PhysObjectWake();

private:

	EHANDLE m_hBurnAttacker;	//For handling afterburn
	EHANDLE m_hInfecAttacker;
	EHANDLE m_hHealer;		//store healers for assists

	//TODO: Maybe something to buff afterburn damage if pyros hit an incindiary, a napalm, and flamethrower the player?

	CTFCSPlayerStateInfo* m_pCurStateInfo;

	CTFCSPlayerAnimState *m_PlayerAnimState;

	CNetworkQAngle( m_angEyeAngles );

	int m_iLastWeaponFireUsercmd;
	CNetworkVar( int, m_iSpawnInterpCounter );
	CNetworkVar( int, m_iPlayerState );


	float m_flNextTeamChangeTime;

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

#endif //TFCS_PLAYER_H
