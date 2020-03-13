#ifndef C_TFCS_PLAYER_H
#define C_TFCS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_playeranimstate.h"
#include "c_baseplayer.h"
#include "baseparticleentity.h"
#include "tfcs_player_shared.h"
#include "tfcs_weapon_base.h"
#include "beamdraw.h"
#include "flashlighteffect.h"

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

	virtual void TeamChange( int iNewTeam ) OVERRIDE;
	static void TeamChangeStatic( int iNewTeam );

	virtual void ClientThink();

	virtual const QAngle& GetRenderAngles();
	virtual const QAngle& EyeAngles();
	virtual void UpdateClientSideAnimation();
	//virtual int DrawModel( int flags );

	// Target ID
	int GetIDTarget() const	{ return m_iIDEntIndex;	}
	void UpdateIDTarget( void );

	int	m_iRealSequence;
	int	  m_iIDEntIndex;

public: // called by shared code

	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );

	CTFCSPlayerShared m_Shared;

	CTFCSWeaponBase* Weapon_OwnsThisID( int iWeaponID );
	CTFCSWeaponBase* GetActiveTFCSWeapon() const;

private:

	QAngle m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;
	ITFCSPlayerAnimState* m_PlayerAnimState;
};

inline C_TFCSPlayer* ToTFCSPlayer( CBaseEntity *pPlayer )
{
	if ( !pPlayer || !pPlayer->IsPlayer() )
		return NULL;

	return static_cast< C_TFCSPlayer* >( pPlayer );
}

#endif //C_TFCS_PLAYER_H