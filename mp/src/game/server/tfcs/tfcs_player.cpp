#include "cbase.h"
#include "tfcs_player.h"
#include "tfcs_shareddefs.h"
#include "multiplayer_animstate.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "team.h"

#define TFCS_PLAYER_MODEL "models/player/scout.mdl"

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS(CTEPlayerAnimEvent, CBaseTempEntity);
	DECLARE_SERVERCLASS();

	CTEPlayerAnimEvent(const char *name) : CBaseTempEntity(name)
	{
	}

	CNetworkHandle(CBasePlayer, m_hPlayer);
	CNetworkVar(int, m_iEvent);
	CNetworkVar(int, m_nData);
};

//IMPLEMENT_SERVERCLASS_ST_NOBASE(CTEPlayerAnimEvent, DT_TEPlayerAnimEvent)
//	SendPropEHandle(SENDINFO(m_hPlayer)),
//	SendPropInt(SENDINFO(m_iEvent), Q_log2(PLAYERANIMEVENT_COUNT) + 1, SPROP_UNSIGNED),
//	SendPropInt(SENDINFO(m_nData), 32),
//END_SEND_TABLE()

//IMPLEMENT_SERVERCLASS_ST(CTFCSPlayer, DT_CTFCSPlayer)
//
//END_SEND_TABLE()

void cc_CreatePredictionError_f()
{
	CBaseEntity *pEnt = CBaseEntity::Instance(1);
	pEnt->SetAbsOrigin(pEnt->GetAbsOrigin() + Vector(63, 0, 0));
}

ConCommand cc_CreatePredictionError("CreatePredictionError", cc_CreatePredictionError_f, "Create a prediction error", FCVAR_CHEAT);

BEGIN_DATADESC(CTFCSPlayer)
DEFINE_THINKFUNC(Think),

DEFINE_FIELD(m_ArmorValue, FIELD_INTEGER),
DEFINE_FIELD(m_MaxArmorValue, FIELD_INTEGER),
END_DATADESC()

LINK_ENTITY_TO_CLASS(player, CTFCSPlayer);
PRECACHE_REGISTER(player);

//BEGIN_SEND_TABLE_NOBASE(CTFCSPlayer, DT_TFCSLocalPlayerExclusive)
//SendPropVector(SENDINFO(m_vecOrigin), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin),
//SendPropFloat(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f),
//
//END_SEND_TABLE()
//
//BEGIN_SEND_TABLE_NOBASE(CTFCSPlayer, DT_TFCSNonLocalPlayerExclusive)
//SendPropVector(SENDINFO(m_vecOrigin), -1, SPROP_COORD_MP_LOWPRECISION | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin),
//SendPropFloat(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f),
//SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN),
//END_SEND_TABLE()
//
//IMPLEMENT_SERVERCLASS_ST(CTFCSPlayer, DT_TFCSPlayer)
//SendPropExclude("DT_BaseAnimating", "m_flPoseParameter"),
//SendPropExclude("DT_BaseAnimating", "m_flPlaybackRate"),
//SendPropExclude("DT_BaseAnimating", "m_nSequence"),
//SendPropExclude("DT_BaseAnimating", "m_nNewSequenceParity"),
//SendPropExclude("DT_BaseAnimating", "m_nResetEventsParity"),
//SendPropExclude("DT_BaseEntity", "m_angRotation"),
//SendPropExclude("DT_BaseAnimatingOverlay", "overlay_vars"),
//SendPropExclude("DT_BaseEntity", "m_vecOrigin"),
//
//// playeranimstate and clientside animation takes care of these on the client
//SendPropExclude("DT_ServerAnimationData", "m_flCycle"),
//SendPropExclude("DT_AnimTimeMustBeFirst", "m_flAnimTime"),
//
//SendPropEHandle(SENDINFO(m_hRagdoll)),
//SendPropInt(SENDINFO(m_iPlayerState), Q_log2(NUM_PLAYER_STATES) + 1, SPROP_UNSIGNED),
//END_SEND_TABLE()

// -------------------------------------------------------------------------------- //
// Ragdoll entities.
// -------------------------------------------------------------------------------- //
class CTFCSRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS(CTFCSRagdoll, CBaseAnimatingOverlay);
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState() { return SetTransmitState(FL_EDICT_ALWAYS); }

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle(CBaseEntity, m_hPlayer);	// networked entity handle 
	CNetworkVector(m_vecRagdollVelocity);
	CNetworkVector(m_vecRagdollOrigin);
};

LINK_ENTITY_TO_CLASS(sdk_ragdoll, CTFCSRagdoll);

//IMPLEMENT_SERVERCLASS_ST_NOBASE(CTFCSRagdoll, DT_SDKRagdoll)
//SendPropVector(SENDINFO(m_vecRagdollOrigin), -1, SPROP_COORD),
//SendPropEHandle(SENDINFO(m_hPlayer)),
//SendPropModelIndex(SENDINFO(m_nModelIndex)),
//SendPropInt(SENDINFO(m_nForceBone), 8, 0),
//SendPropVector(SENDINFO(m_vecForce), -1, SPROP_NOSCALE),
//SendPropVector(SENDINFO(m_vecRagdollVelocity))
//END_SEND_TABLE()

CTFCSPlayer::CTFCSPlayer(){}

CTFCSPlayer::~CTFCSPlayer()
{
	//m_PlayerAnimState->Release();
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

