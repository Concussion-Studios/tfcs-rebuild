//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef TFCS_PLAYERANIMSTATE_H
#define TFCS_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "multiplayer_animstate.h"
#include "base_playeranimstate.h"
#if defined( CLIENT_DLL )
class C_TFCSPlayer;
#define CTFCSPlayer C_TFCSPlayer
#else
class CTFCSPlayer;
#endif

// ------------------------------------------------------------------------------------------------ //
// CTFCSPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CTFCSPlayerAnimState : public CMultiPlayerAnimState
{
public:
	
	DECLARE_CLASS( CTFCSPlayerAnimState, CMultiPlayerAnimState );

	CTFCSPlayerAnimState();
	CTFCSPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
	~CTFCSPlayerAnimState();

	void InitTFCSAnimState( CTFCSPlayer *pPlayer );
	CTFCSPlayer *GetTFCSPlayer( void ) { return m_pTFCSPlayer; }

	virtual void ClearAnimationState();
	virtual Activity TranslateActivity( Activity actDesired );
	virtual void Update( float eyeYaw, float eyePitch );

	void	DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	bool	HandleMoving( Activity &idealActivity );
	bool	HandleJumping( Activity &idealActivity );
	bool	HandleDucking( Activity &idealActivity );
	bool	HandleSwimming( Activity &idealActivity );

	virtual float GetCurrentMaxGroundSpeed();

protected:
	CModAnimConfig		m_AnimConfig;

private:
	//Tony; temp till 9way!
	bool						SetupPoseParameters( CStudioHdr *pStudioHdr );
	virtual void				EstimateYaw( void );
	virtual void				ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimPitch( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimYaw( CStudioHdr *pStudioHdr );
	void						ComputePlaybackRate();
	
	CTFCSPlayer   *m_pTFCSPlayer;
	bool		m_bInAirWalk;
	float		m_flHoldDeployedPoseUntilTime;
};

CTFCSPlayerAnimState *CreateTFCSPlayerAnimState( CTFCSPlayer *pPlayer );

#endif // TFCS_PLAYERANIMSTATE_H