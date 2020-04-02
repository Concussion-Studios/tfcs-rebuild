//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef C_TFCS_PLAYER_H
#define C_TFCS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_playeranimstate.h"
#include "c_baseflex.h"
#include "c_basehlplayer.h"
#include "c_basetempentity.h"
#include "baseparticleentity.h"
#include "tfcs_player_shared.h"
#include "tfcs_weapon_base.h"
#include "beamdraw.h"
#include "flashlighteffect.h"
#include "props_shared.h"

// -------------------------------------------------------------------------------- //
// Tony; m_pFlashlightEffect is private, so just subclass. We may want to do some more stuff with it later anyway.
// -------------------------------------------------------------------------------- //
class CTFCSFlashlightEffect : public CFlashlightEffect
{
public:
	CTFCSFlashlightEffect(int nIndex = 0) : CFlashlightEffect( nIndex  ) {}
	~CTFCSFlashlightEffect() {};

	virtual void UpdateLight( const Vector &vecPos, const Vector &vecDir, const Vector &vecRight, const Vector &vecUp, int nDistance );
};

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //
class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType );

public:
	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

// -------------------------------------------------------------------------------- //
// Ragdoll entities.
// -------------------------------------------------------------------------------- //
class C_TFCSRagdoll : public C_BaseFlex
{
public:
	DECLARE_CLASS( C_TFCSRagdoll, C_BaseFlex );
	DECLARE_CLIENTCLASS();
	
	C_TFCSRagdoll() {}
	~C_TFCSRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

	void ClientThink( void );
	void StartFadeOut( float fDelay );
	void EndFadeOut();

	// Shadows
	virtual ShadowType_t ShadowCastType( void );

	void ImpactTrace( trace_t* pTrace, int iDamageType, const char* pCustomImpactName );
	void UpdateOnRemove( void );

	bool IsRagdollVisible();
	virtual void SetupWeights( const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights );

private:

	C_TFCSRagdoll( const C_TFCSRagdoll& ) {}

	void Interp_Copy( C_BaseAnimatingOverlay* pDestinationEntity );
	void CreateTFCSRagdoll( void );
	void CreateTFCSGibs( void );

private:

	float m_fDeathTime;
	bool  m_bFadingOut;

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
	bool  m_bGib;
	bool  m_bBurning;
};

// -------------------------------------------------------------------------------- //
// Purpose:
// -------------------------------------------------------------------------------- //
class C_TFCSPlayer : public C_BaseHLPlayer
{
public:
	DECLARE_CLASS( C_TFCSPlayer, C_BaseHLPlayer );

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();


	C_TFCSPlayer();
	~C_TFCSPlayer( void );

	void ClientThink( void );

	static C_TFCSPlayer* GetLocalTFCSPlayer();
	
	virtual int DrawModel( int flags );
	virtual void AddEntity( void );
	virtual Vector GetObserverCamOrigin( void );

	// Should this object cast shadows?
	virtual ShadowType_t ShadowCastType( void );
	virtual void GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType );
	virtual void GetRenderBounds( Vector& theMins, Vector& theMaxs);
	virtual bool GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const;
	virtual bool ShouldReceiveProjectedTextures( int flags );
	virtual C_BaseAnimating *BecomeRagdollOnClient() { return nullptr; }
	virtual const QAngle& GetRenderAngles();
	virtual bool ShouldDraw( void );
	virtual void OnDataChanged( DataUpdateType_t type );
	virtual void ItemPreFrame( void );
	virtual void ItemPostFrame( void );
	virtual void NotifyShouldTransmit( ShouldTransmitState_t state );
	virtual void CreateLightEffects( void ) {}
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void PreThink( void );
	virtual void Simulate();
	virtual void DoImpactEffect( trace_t &tr, int nDamageType );
	IRagdoll* GetRepresentativeRagdoll() const;
	virtual const QAngle& EyeAngles( void );

	void	UpdateLookAt( void );
	int		GetIDTarget() const { return m_iIDEntIndex;	}
	void	UpdateIDTarget( void );

	virtual void UpdateClientSideAnimation();
	virtual void CalculateIKLocks( float currentTime );

	static void RecvProxy_CycleLatch( const CRecvProxyData *pData, void *pStruct, void *pOut );

	virtual float GetServerIntendedCycle() { return m_flServerCycle; }
	virtual void SetServerIntendedCycle( float cycle ) { m_flServerCycle = cycle; }

	void InitializePoseParams( void );
	virtual CTFCSPlayerAnimState* GetAnimState() { return m_PlayerAnimState; }

	// Player avoidance
	void AvoidPlayers( CUserCmd *pCmd );
	float m_fNextThinkPushAway;
	virtual bool CreateMove( float flInputSampleTime, CUserCmd *pCmd );
	
	// Gibs.
	void InitPlayerGibs( void );
	bool CreatePlayerGibs( const Vector& vecOrigin, const Vector& vecVelocity, float flImpactScale, bool bBurning );
	CUtlVector<EHANDLE>* GetSpawnedGibs( void ) { return &m_hSpawnedGibs; }

public: // called by shared code

	virtual Vector GetAutoaimVector( float flDelta );
	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual void TraceAttack( const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator );
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;
	virtual CStudioHdr *OnNewModel( void );		//Tony; when model is changed, need to init some stuff.

	CTFCSPlayerShared m_Shared;

	CTFCSWeaponBase* Weapon_OwnsThisID( int iWeaponID );
	CTFCSWeaponBase* GetActiveTFCSWeapon() const;

	CNetworkVar( float, m_ArmorClass );
	CNetworkVar( int, m_MaxArmor );
	CNetworkVar( float, m_flConcussTime );

private:
	
	C_TFCSPlayer( const C_TFCSPlayer & );
	CTFCSPlayerAnimState *m_PlayerAnimState;

	QAngle	m_angEyeAngles;

	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	EHANDLE	m_hRagdoll;

	int	m_headYawPoseParam;
	int	m_headPitchPoseParam;
	float m_headYawMin;
	float m_headYawMax;
	float m_headPitchMin;
	float m_headPitchMax;

	bool m_isInit;
	Vector m_vLookAtTarget;

	float m_flLastBodyYaw;
	float m_flCurrentHeadYaw;
	float m_flCurrentHeadPitch;

	int	  m_iIDEntIndex;

	CountdownTimer m_blinkTimer;

	int	  m_iSpawnInterpCounter;
	int	  m_iSpawnInterpCounterCache;

	virtual void	UpdateFlashlight( void ); //Tony; override.
	void ReleaseFlashlight( void );
	Beam_t	*m_pFlashlightBeam;

	CTFCSFlashlightEffect *m_pTFCSFlashLightEffect;
	
	// Gibs.
	CUtlVector<breakmodel_t> m_aGibs;
	EHANDLE m_hFirstGib;
	CUtlVector<EHANDLE> m_hSpawnedGibs;

	int m_cycleLatch; // The animation cycle goes out of sync very easily. Mostly from the player entering/exiting PVS. Server will frequently update us with a new one.
	float m_flServerCycle;
	
	friend class C_TFCSRagdoll;
	friend class CTFCSPlayerAnimState;
};

inline C_TFCSPlayer* ToTFCSPlayer( C_BaseEntity* pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return nullptr;

	return static_cast<C_TFCSPlayer*>( pEntity );
}

inline C_TFCSPlayer* ToTFCSPlayer( C_BasePlayer* pPlayer )
{
	return static_cast<C_TFCSPlayer*>( pPlayer );
}

#endif //TFCSPlayer_H
