//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TFC_PLAYERANIMSTATE_H
#define TFC_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"
#include "multiplayer_animstate.h"

#if defined( CLIENT_DLL )
class C_TFCPlayer;
#define CTFCPlayer C_TFCPlayer
#else
class CTFCPlayer;
#endif

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CTFCPlayerAnimState : public CMultiPlayerAnimState
{
public:
	
	DECLARE_CLASS( CTFCPlayerAnimState, CMultiPlayerAnimState );

	CTFCPlayerAnimState();
	CTFCPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
	~CTFCPlayerAnimState();

	void InitTFCAnimState( CTFCPlayer *pPlayer );
	CTFCPlayer *GetTFCPlayer( void )							{ return m_pTFCPlayer; }

	virtual void ClearAnimationState();
	virtual Activity TranslateActivity( Activity actDesired );
	virtual void Update( float eyeYaw, float eyePitch );

	void	DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	bool	HandleMoving( Activity &idealActivity );
	bool	HandleJumping( Activity &idealActivity );
	bool	HandleDucking( Activity &idealActivity );
	bool	HandleSwimming( Activity &idealActivity );

	//Tony; overriding because the TFC Player models pose parameter is flipped the opposite direction
	virtual void		ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );

	virtual Activity CalcMainActivity();	

private:
	
	CTFCPlayer   *m_pTFCPlayer;
	bool		m_bInAirWalk;

	float		m_flHoldDeployedPoseUntilTime;
};

CTFCPlayerAnimState *CreateTFCPlayerAnimState( CTFCPlayer *pPlayer );



#endif // TFC_PLAYERANIMSTATE_H
