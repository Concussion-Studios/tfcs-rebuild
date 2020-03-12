#ifndef TFCS_PLAYERANIMSTATE_H
#define TFCS_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "iplayeranimstate.h"
#include "base_playeranimstate.h"
#include "studio.h"

#ifdef CLIENT_DLL
	#define CTFCSPlayer C_TFCSPlayer
#else
	class CTFCSPlayer;
#endif

// When moving this fast, he plays run anim.
#define ARBITRARY_RUN_SPEED		175.0f

#define MAX_STANDING_RUN_SPEED	320
#define MAX_CROUCHED_RUN_SPEED	110

enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_FIRE_GUN=0,
	PLAYERANIMEVENT_THROW_GRENADE,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_RELOAD,
	PLAYERANIMEVENT_DIE,
	
	PLAYERANIMEVENT_COUNT
};

class ITFCSPlayerAnimState : virtual public IPlayerAnimState
{
public:
	// This is called by both the client and the server in the same way to trigger events for
	// players firing, jumping, throwing grenades, etc.
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData ) = 0;
};

class CTFCSPlayerAnimState : public ITFCSPlayerAnimState, public CBasePlayerAnimState
{
public:

#ifdef CLIENT_DLL
	friend class C_TFCSPlayer;
	typedef C_TFCSPlayer OuterClass;
#else
	friend class CTFCSPlayer;
	typedef CTFCSPlayer OuterClass;
#endif
	
	DECLARE_CLASS( CTFCSPlayerAnimState, CBasePlayerAnimState );

	CTFCSPlayerAnimState();
	void Init( OuterClass* pPlayer );

	// This is called by both the client and the server in the same way to trigger events for
	// players firing, jumping, throwing grenades, etc.
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData );
	virtual int CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle );
	virtual float SetOuterBodyYaw( float flValue );
	virtual Activity CalcMainActivity();
	virtual float GetCurrentMaxGroundSpeed();
	virtual void ClearAnimationState();
	virtual bool ShouldUpdateAnimState();
	virtual int SelectWeightedSequence( Activity activity ) ;

	float CalcMovementPlaybackRate( bool *bIsMoving );

	virtual void ComputePoseParam_BodyPitch( CStudioHdr *pStudioHdr );


private:
	
	const char* GetWeaponSuffix();
	bool HandleJumping();
	bool HandleDeath( Activity *deathActivity );


private:
	
	OuterClass *m_pOuter;
	
	bool m_bJumping;
	bool m_bFirstJumpFrame;
	float m_flJumpStartTime;

	bool m_bFiring;
	float m_flFireStartTime;

	bool m_bDying;
	Activity m_DeathActivity;
};

// If this is set, then the game code needs to make sure to send player animation events
// to the local player if he's the one being watched.
extern ConVar cl_showanimstate;

#endif // TFCS_PLAYERANIMSTATE_H