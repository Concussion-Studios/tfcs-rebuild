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

ITFCSPlayerAnimState* CreatePlayerAnimState( CTFCSPlayer *pPlayer );

// If this is set, then the game code needs to make sure to send player animation events
// to the local player if he's the one being watched.
extern ConVar cl_showanimstate;

#endif // TFCS_PLAYERANIMSTATE_H