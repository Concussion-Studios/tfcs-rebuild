#ifndef C_TFCS_PLAYER_H
#define C_TFCS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_playeranimstate.h"
#include "c_baseplayer.h"
#include "c_baseflex.h"
#include "baseparticleentity.h"
#include "tfcs_player_shared.h"
#include "tfcs_weapon_base.h"
#include "beamdraw.h"
#include "flashlighteffect.h"
#include "c_basetempentity.h"
#include "props_shared.h"

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

class C_TFCSPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_TFCSPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_TFCSPlayer();
	~C_TFCSPlayer();

	static C_TFCSPlayer* GetLocalTFCSPlayer();

	virtual void Respawn(); // Spawn() is called once, this is called every time
	virtual void ClientThink();
	virtual Vector GetObserverCamOrigin( void );
	virtual void ItemPreFrame( void );
	virtual void ItemPostFrame( void );
	virtual void OnDataChanged( DataUpdateType_t type );
	virtual void DoImpactEffect( trace_t& tr, int nDamageType );
	virtual void CreateLightEffects( void ) {}
	virtual bool ShouldDraw( void );

	// Player avoidance
	void AvoidPlayers( CUserCmd* pCmd );
	float m_fNextThinkPushAway;
	virtual bool CreateMove( float flInputSampleTime, CUserCmd* pCmd );

	// Implement ITFCSPlayerAnimState
	virtual const QAngle& GetRenderAngles();
	virtual const QAngle& EyeAngles();
	virtual void UpdateClientSideAnimation();
	virtual int DrawModel( int flags );
	virtual CStudioHdr* OnNewModel( void );
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	static void RecvProxy_CycleLatch( const CRecvProxyData* pData, void* pStruct, void* pOut );
	virtual float GetServerIntendedCycle() { return m_flServerCycle; }
	virtual void SetServerIntendedCycle( float cycle ) { m_flServerCycle = cycle; }
	virtual ITFCSPlayerAnimState* GetAnimState() { return m_PlayerAnimState; }

	// Shadows
	virtual ShadowType_t ShadowCastType( void );
	virtual void GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType );
	virtual void GetRenderBounds( Vector& theMins, Vector& theMaxs);
	virtual bool GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const;
	virtual bool ShouldReceiveProjectedTextures( int flags );

	// Implement multiplayer ragdolls
	virtual C_BaseAnimating* BecomeRagdollOnClient();
	virtual IRagdoll* GetRepresentativeRagdoll() const;

	// Target ID

	int GetIDTarget() const	{ return m_iIDEntIndex;	}
	void UpdateIDTarget( void );

	// Gibs.
	void InitPlayerGibs( void );
	bool CreatePlayerGibs( const Vector& vecOrigin, const Vector& vecVelocity, float flImpactScale, bool bBurning );
	CUtlVector<EHANDLE>* GetSpawnedGibs( void ) { return &m_hSpawnedGibs; }

	virtual void TeamChange( int iNewTeam ) OVERRIDE;
	static void TeamChangeStatic( int iNewTeam );

public: // called by shared code

	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual void TraceAttack( const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator );
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;

	CTFCSPlayerShared m_Shared;

	CTFCSWeaponBase* Weapon_OwnsThisID( int iWeaponID );
	CTFCSWeaponBase* GetActiveTFCSWeapon() const;

	CNetworkVar( float, m_ArmorClass );
	CNetworkVar( int, m_MaxArmor );
	CNetworkVar( float, m_flConcussTime );

private:

	ITFCSPlayerAnimState* m_PlayerAnimState;

	QAngle m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	int m_cycleLatch; // The animation cycle goes out of sync very easily. Mostly from the player entering/exiting PVS. Server will frequently update us with a new one.
	float m_flServerCycle;

	bool m_bSpawnInterpCounter;
	bool m_bSpawnInterpCounterCache;

	EHANDLE	m_hRagdoll;

	// Gibs.
	CUtlVector<breakmodel_t> m_aGibs;
	EHANDLE m_hFirstGib;
	CUtlVector<EHANDLE> m_hSpawnedGibs;

	int	m_iRealSequence;
	int	  m_iIDEntIndex;

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

#endif //C_TFCS_PLAYER_H