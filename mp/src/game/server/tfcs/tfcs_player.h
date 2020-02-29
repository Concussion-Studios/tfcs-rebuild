#ifndef TFCS_PLAYER_H
#define TFCS_PLAYER_H
#pragma once

#include "basemultiplayerplayer.h"
//#include "tfcs_shareddefs.h"

class CTFCSPlayer;

class CTFCSPlayer : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS(CTFCSPlayer, CBaseMultiplayerPlayer);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTFCSPlayer();
	~CTFCSPlayer();

	static CTFCSPlayer	*CreatePlayer(const char *className, edict_t *ed);
	static CTFCSPlayer	*Instance(int iEnt);

	virtual void		Precache();
	virtual void		Spawn();
	virtual void		ForceRespawn();
	virtual void		PreThink();
	virtual void		PostThink();
	virtual int			OnTakeDamage(const CTakeDamageInfo &info);
	virtual int			OnTakeDamage_Alive(const CTakeDamageInfo &info);
	virtual void		Event_Killed(const CTakeDamageInfo &info);
	virtual bool		ClientCommand(const CCommand &args);
	virtual void		ChangeTeam(int iTeamNum);
	virtual int			TakeHealth(float flHealth);
	int					TakeArmor(float flArmor);
	void				SetArmorClass(float flClass);
	void				Concuss();
	void				Cripple(int iCrippleLevel);
	int					GetCrippleLevel();
	void				SaveMe();
	void				Burn();
	void				Infect();
	
	CNetworkQAngle(m_angEyeAngles);

	CNetworkHandle(CBaseEntity, m_hRagdoll);

private:
	void				GiveDefaultItems();
	void				TFCSPlayerThink();

	//TFCS related vars
	CNetworkVar(float, m_flArmorClass);
	CNetworkVar(int, m_iArmor);
	CNetworkVar(int, m_iMaxArmor);
	CNetworkVar(float, m_flConcussTime);
	CNetworkVar(float, m_flCrippleTime);
	CNetworkVar(int, m_iCrippleLevel);
	EHANDLE				m_hBurnAttacker;
	EHANDLE				m_hInfecAttacker;

	CNetworkVar(int, m_iPlayerState);
};

inline CTFCSPlayer *ToTFCSPlayer(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return static_cast<CTFCSPlayer*>(pEntity);
}

//TODO add playerclassinfo

#endif //TFCS_PLAYER_H