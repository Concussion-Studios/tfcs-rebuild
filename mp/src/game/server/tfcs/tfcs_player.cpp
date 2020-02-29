#include "cbase.h"
#include "tfcs_player.h"
#include "tfcs_shareddefs.h"
#include "multiplayer_animstate.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "team.h"

#define TFCS_PLAYER_MODEL "models/player/scout.mdl"

void cc_CreatePredictionError_f()
{
	CBaseEntity *pEnt = CBaseEntity::Instance(1);
	pEnt->SetAbsOrigin(pEnt->GetAbsOrigin() + Vector(63, 0, 0));
}

ConCommand cc_CreatePredictionError("CreatePredictionError", cc_CreatePredictionError_f, "Create a prediction error", FCVAR_CHEAT);

BEGIN_DATADESC(CTFCSPlayer)
DEFINE_THINKFUNC(Think),

DEFINE_FIELD(m_flArmorClass, FIELD_FLOAT),
DEFINE_FIELD(m_iArmor, FIELD_INTEGER),
DEFINE_FIELD(m_iMaxArmor, FIELD_FLOAT),
DEFINE_FIELD(m_flConcussTime, FIELD_FLOAT),
DEFINE_FIELD(m_flCrippleTime, FIELD_FLOAT),
DEFINE_FIELD(m_iCrippleLevel, FIELD_INTEGER),
END_DATADESC()

LINK_ENTITY_TO_CLASS(player, CTFCSPlayer);
PRECACHE_REGISTER(player);

BEGIN_SEND_TABLE_NOBASE(CTFCSPlayer, DT_TFCSLocalPlayerExclusive)
SendPropVector(SENDINFO(m_vecOrigin), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin),
SendPropInt(SENDINFO(m_flArmorClass)),
SendPropInt(SENDINFO(m_iArmor), 8, SPROP_UNSIGNED),
SendPropInt(SENDINFO(m_iMaxArmor), 8, SPROP_UNSIGNED),
SendPropFloat(SENDINFO(m_flConcussTime)),
END_SEND_TABLE()


CTFCSPlayer::CTFCSPlayer(){}

CTFCSPlayer::~CTFCSPlayer()
{
	
}

void CTFCSPlayer::Precache()
{
	BaseClass::Precache();
}

void CTFCSPlayer::Spawn()
{
	BaseClass::Spawn();
}

void CTFCSPlayer::GiveDefaultItems()
{

}

void CTFCSPlayer::ForceRespawn()
{
	BaseClass::ForceRespawn();
}

void CTFCSPlayer::PreThink()
{
	BaseClass::PreThink();
}

void CTFCSPlayer::TFCSPlayerThink()
{
	BaseClass::Think();
}

void CTFCSPlayer::PostThink()
{

}

int CTFCSPlayer::OnTakeDamage(const CTakeDamageInfo &info)
{
	return BaseClass::OnTakeDamage(info);
}

int CTFCSPlayer::OnTakeDamage_Alive(const CTakeDamageInfo &info)
{
	return BaseClass::OnTakeDamage_Alive(info);
}

void CTFCSPlayer::Event_Killed(const CTakeDamageInfo &info)
{
	BaseClass::Event_Killed(info);
}

bool CTFCSPlayer::ClientCommand(const CCommand &args)
{
	return false;
}

void CTFCSPlayer::ChangeTeam(int iTeamNum)
{

}

int CTFCSPlayer::TakeHealth(float flHealth)
{
	return 0;
}

int CTFCSPlayer::TakeArmor(float flArmor)
{
	return 0;
}

void CTFCSPlayer::SetArmorClass(float flArmorClass)
{

}

CTFCSPlayer *CTFCSPlayer::CreatePlayer(const char *className, edict_t *ed)
{
	CTFCSPlayer::s_PlayerEdict = ed;
	return (CTFCSPlayer*)CreateEntityByName(className);
}

