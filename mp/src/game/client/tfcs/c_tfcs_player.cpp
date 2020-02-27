#include "cbase.h"
#include "c_tfcs_player.h"
#include "c_basetempentity.h"
#include "multiplayer_animstate.h"

#include "tier0/memdbgon.h"

#ifdef CTFCSPlayer
#undef CTFCSPlayer
#endif

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //
class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS(C_TEPlayerAnimEvent, C_BaseTempEntity);
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate(DataUpdateType_t updateType)
	{
		// Create the effect.
		C_TFCSPlayer *pPlayer = dynamic_cast< C_TFCSPlayer* >(m_hPlayer.Get());
		if (pPlayer && !pPlayer->IsDormant())
		{
			pPlayer->DoAnimationEvent((PlayerAnimEvent_t)m_iEvent.Get(), m_nData);
		}
	}

public:
	CNetworkHandle(CBasePlayer, m_hPlayer);
	CNetworkVar(int, m_iEvent);
	CNetworkVar(int, m_nData);
};

//IMPLEMENT_CLIENTCLASS_EVENT(C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent);
//
//BEGIN_RECV_TABLE_NOBASE(C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent)
//RecvPropEHandle(RECVINFO(m_hPlayer)),
//RecvPropInt(RECVINFO(m_iEvent)),
//RecvPropInt(RECVINFO(m_nData))
//END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE(C_TFCSPlayer, DT_TFCSLocalPlayerExclusive)
RecvPropVector(RECVINFO_NAME(m_vecNetworkOrigin, m_vecOrigin)),
RecvPropFloat(RECVINFO(m_angEyeAngles[0])),
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE(C_TFCSPlayer, DT_TFCSNonLocalPlayerExclusive)
RecvPropVector(RECVINFO_NAME(m_vecNetworkOrigin, m_vecOrigin)),
RecvPropFloat(RECVINFO(m_angEyeAngles[0])),
RecvPropFloat(RECVINFO(m_angEyeAngles[1])),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT(C_TFCSPlayer, DT_TFCSPlayer, CTFCSPlayer)

RecvPropEHandle(RECVINFO(m_hRagdoll)),
RecvPropInt(RECVINFO(m_iPlayerState)),
END_RECV_TABLE()

class C_TFCSRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS(C_TFCSRagdoll, C_BaseAnimatingOverlay)
	DECLARE_CLIENTCLASS();

	C_TFCSRagdoll();
	~C_TFCSRagdoll();

	virtual void OnDataChanged(DataUpdateType_t type);

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

	void ImpactTrace(trace_t *pTrace, int iDamageType, const char *pCustomImpactName);
	void UpdateOnRemove(void);
	virtual void SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights);

private:
	C_TFCSRagdoll(const C_TFCSRagdoll &) {}
	void Interp_Copy(C_BaseAnimatingOverlay *pDestinationEntity);
	void CreateTFCSRagdoll(void);

	EHANDLE m_hPlayer;
	CNetworkVector(m_vecRagdollVelocity);
	CNetworkVector(m_vecRagdollOrigin);
};

IMPLEMENT_CLIENTCLASS_DT_NOBASE(C_TFCSRagdoll, DT_TFCSRagdoll, CTFCSRagdoll)
RecvPropVector(RECVINFO(m_vecRagdollOrigin)),
RecvPropEHandle(RECVINFO(m_hPlayer)),
RecvPropInt(RECVINFO(m_nModelIndex)),
RecvPropInt(RECVINFO(m_nForceBone)),
RecvPropVector(RECVINFO(m_vecForce)),
RecvPropVector(RECVINFO(m_vecRagdollVelocity))
END_RECV_TABLE()

C_TFCSRagdoll::C_TFCSRagdoll()
{

}

C_TFCSRagdoll::~C_TFCSRagdoll()
{
	PhysCleanupFrictionSounds(this);

	if (m_hPlayer)
		m_hPlayer->CreateModelInstance();
}

void C_TFCSRagdoll::Interp_Copy(C_BaseAnimatingOverlay *pSourceEntity)
{
	if (!pSourceEntity)
		return;

	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for (int i = 0; i < pDest->m_Entries.Count(); i++)
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
		const char *pszName = pDestEntry->watcher->GetDebugName();
		for (int j = 0; j < pSrc->m_Entries.Count(); j++)
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
			if (!Q_strcmp(pSrcEntry->watcher->GetDebugName(), pszName))
			{
				pDestEntry->watcher->Copy(pSrcEntry->watcher);
				break;
			}
		}
	}
}

void FX_BloodSpray(const Vector &origin, const Vector &normal, float scale, unsigned char r, unsigned char g, unsigned char b, int flags);
void C_TFCSRagdoll::ImpactTrace(trace_t *pTrace, int iDamageType, const char *pCustomImpactName)
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if (!pPhysicsObject)
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if (iDamageType == DMG_BLAST)
	{
		dir *= 4000;  // adjust impact strenght

		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter(dir);
	}
	else
	{
		Vector hitpos;

		VectorMA(pTrace->startpos, pTrace->fraction, dir, hitpos);
		VectorNormalize(dir);

		// Blood spray!
		FX_BloodSpray(hitpos, dir, 3, 72, 0, 0, FX_BLOODSPRAY_ALL);

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset(dir, hitpos);
		//Tony; throw in some bleeds! - just use a generic value for damage.
		TraceBleed(40, dir, pTrace, iDamageType);
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}

void C_TFCSRagdoll::OnDataChanged(DataUpdateType_t type)
{
	BaseClass::OnDataChanged(type);

	if (type == DATA_UPDATE_CREATED)
		CreateTFCSRagdoll();
}

IRagdoll *C_TFCSRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

void C_TFCSRagdoll::UpdateOnRemove(void)
{
	VPhysicsSetObject(NULL);

	BaseClass::UpdateOnRemove();
}

void C_TFCSRagdoll::SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights)
{
	BaseClass::SetupWeights(pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights);

	static float destweight[128];
	static bool bIsInited = false;

	CStudioHdr *hdr = GetModelPtr();
	if (!hdr)
		return;

	int nFlexDescCount = hdr->numflexdesc();
	if (nFlexDescCount)
	{
		Assert(!pFlexDelayedWeights);
		memset(pFlexWeights, 0, nFlexWeightCount * sizeof(float));
	}

	if (m_iEyeAttachment > 0)
	{
		matrix3x4_t attToWorld;
		if (GetAttachment(m_iEyeAttachment, attToWorld))
		{
			Vector local, tmp;
			local.Init(1000.0f, 0.0f, 0.0f);
			VectorTransform(local, attToWorld, tmp);
			modelrender->SetViewTarget(GetModelPtr(), GetBody(), tmp);
		}
	}
}

C_TFCSPlayer::C_TFCSPlayer() : m_iv_angEyeAngles("C_TFCSPlayer::m_iv_angEyeAngles")
{
	//m_PlayerAnimState = CreateTFCSPlayerAnimState(this);
	//m_Shared.Init(this);

	m_angEyeAngles.Init();
	AddVar(&m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR);

	//m_fNextThinkPushAway = 0.0f;
}

C_TFCSPlayer::~C_TFCSPlayer()
{
	
}

C_TFCSPlayer* C_TFCSPlayer::GetLocalTFCSPlayer()
{
	return ToTFCSPlayer(C_BasePlayer::GetLocalPlayer());
}

void C_TFCSPlayer::DoAnimationEvent(PlayerAnimEvent_t event, int nData)
{
	if (IsLocalPlayer())
	{
		if ((prediction->InPrediction() && !prediction->IsFirstTimePredicted()))
			return;
	}

	MDLCACHE_CRITICAL_SECTION();
	m_PlayerAnimState->DoAnimationEvent(event, nData);
}