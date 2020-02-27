#ifndef C_TFCS_PLAYER_H
#define C_TFCS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_playeranimstate.h"
#include "c_baseplayer.h"
#include "baseparticleentity.h"
//#include tfcs_player_shared.h
#include "beamdraw.h"
#include "flashlighteffect.h"

class C_TFCSPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS(C_TFCSPlayer, C_BasePlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_TFCSPlayer();
	~C_TFCSPlayer();

	static C_TFCSPlayer* GetLocalTFCSPlayer();

	virtual const QAngle& GetRenderAngles();
	//virtual int DrawModel(int flags);
	//virtual void DoAnimationEvent(PlayerAnimEvent_t event, int nData = 0);

	//CTFCSPlayerAnimState *m_PlayerAnimState;
	QAngle m_angEyeAngles;
	CInterpolatedVar<QAngle> m_iv_angEyeAngles;
};

inline C_TFCSPlayer* ToTFCSPlayer(CBaseEntity *pPlayer)
{
	if (!pPlayer || !pPlayer->IsPlayer())
		return NULL;

	return static_cast< C_TFCSPlayer* >(pPlayer);
}

#endif //C_TFCS_PLAYER_H