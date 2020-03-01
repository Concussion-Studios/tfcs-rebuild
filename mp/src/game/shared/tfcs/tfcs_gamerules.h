#ifndef TFCS_GAMERULES_H
#define TFCS_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "teamplayroundbased_gamerules.h"
#include "convar.h"
#include "gamevars_shared.h"
#include "tfcs_weapon_base.h"
#include "GameEventListener.h"

#ifdef CLIENT_DLL
#include "c_baseplayer.h"
#else
#include "player.h"
#include "utlqueue.h"
#endif

#ifdef CLIENT_DLL
#define CTFCSGameRules C_TFCSGameRules
#define CTFCSGameRulesProxy C_TFCSGameRulesProxy
#endif

class CTFCSGameRulesProxy : public CTeamplayRoundBasedRulesProxy
{
public:
	DECLARE_CLASS(CTFCSGameRulesProxy, CTeamplayRoundBasedRulesProxy);
	DECLARE_NETWORKCLASS();
};

class CTFCSGameRules : public CTeamplayRoundBasedRules
{
public:
	DECLARE_CLASS(CTFCSGameRules, CTeamplayRoundBasedRules);
	CTFCSGameRules();
	virtual ~CTFCSGameRules();

	virtual bool		ShouldCollide(int collisionGroup0, int collisionGroup1);

	virtual bool		IsTeamplay(void) { return false; }

#ifdef CLIENT_DLL
	DECLARE_CLIENTCLASS_NOBASE();
#else
	DECLARE_SERVERCLASS_NOBASE();

	virtual void RadiusDamage(const CTakeDamageInfo &info, const Vector &vecSrc, float flRadius, int iClassIgnore, CBaseEntity *pEntityIgnore);

	virtual bool ClientCommand(CBaseEntity *pEdict, const CCommand &args);
	virtual void Think();

	virtual const char *GetChatPrefix(bool bTeamOnly, CBasePlayer *pPlayer);

	//TODO: Change this fake key to a real key
	virtual const unsigned char *GetEncryptionKey(void) { return (unsigned char *)"a1b2c3d4"; }

	CBaseEntity *GetPlayerSpawnSpot(CBasePlayer *pPlayer);
	bool IsSpawnPointValid(CBaseEntity *pSpot, CBasePlayer *pPlayer);
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
#endif
private:

protected:
	float m_flGameStartTime;
	CNetworkVar(float, m_flRoundStartTime);
};

inline CTFCSGameRules* TFCSGameRules()
{
	return static_cast<CTFCSGameRules *>(g_pGameRules);
}
#endif //TFCS_GAMERULES_H