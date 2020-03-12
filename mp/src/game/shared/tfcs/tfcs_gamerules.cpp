#include "cbase.h"
#include "tfcs_gamerules.h"
#include "tfcs_weapon_base.h"
#include "KeyValues.h"
#include "ammodef.h"

#ifdef CLIENT_DLL
	//#include "c_tfcs_player.h"
#else
	#include "voice_gamemgr.h"
	#include "team.h"
	//#include "tfcs_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifndef CLIENT_DLL
class CSpawnPoint : public CPointEntity
{
public:
	bool IsDisabled() { return m_bDisabled; }
	void InputEnable( inputdata_t &inputdata ) { m_bDisabled = false; }
	void InputDisable( inputdata_t &inputdata ) { m_bDisabled = true; }

private:
	bool m_bDisabled;
	DECLARE_DATADESC();
};

BEGIN_DATADESC( CSpawnPoint )

	// Keyfields
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),

END_DATADESC();

LINK_ENTITY_TO_CLASS( info_player_deathmatch, CSpawnPoint );
LINK_ENTITY_TO_CLASS( info_player_blue, CSpawnPoint );
LINK_ENTITY_TO_CLASS( info_player_red, CSpawnPoint );
LINK_ENTITY_TO_CLASS( info_player_yellow, CSpawnPoint );
LINK_ENTITY_TO_CLASS( info_player_green, CSpawnPoint );
#endif

REGISTER_GAMERULES_CLASS( CTFCSGameRules );

BEGIN_NETWORK_TABLE_NOBASE( CTFCSGameRules, DT_TFCSGameRules )
#ifdef CLIENT_DLL
	RecvPropFloat( RECVINFO( m_flRoundStartTime ) ),
#else
	SendPropFloat( SENDINFO( m_flRoundStartTime ), 32, SPROP_NOSCALE ),
#endif
END_NETWORK_TABLE();

LINK_ENTITY_TO_CLASS( tfcs_gamerules, CTFCSGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( TFCSGameRulesProxy, DT_TFCSGameRulesProxy )

#ifdef CLIENT_DLL
void RecvProxy_TFCSGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
{
	CTFCSGameRules *pRules = TFCSGameRules();
	Assert( pRules );
	*pOut = pRules;
}

BEGIN_RECV_TABLE( CTFCSGameRulesProxy, DT_TFCSGameRulesProxy )
	RecvPropDataTable( "tfcs_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSGameRules ), RecvProxy_TFCSGameRules )
END_RECV_TABLE()
#else
void *SendProxy_TFCSGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
{
	CTFCSGameRules *pRules = TFCSGameRules();
	Assert( pRules );
	pRecipients->SetAllRecipients();
	return pRules;
}

BEGIN_SEND_TABLE( CTFCSGameRulesProxy, DT_TFCSGameRulesProxy )
	SendPropDataTable( "tfcs_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_TFCSGameRules ), SendProxy_TFCSGameRules )
END_SEND_TABLE()
#endif

#ifndef CLIENT_DLL
ConVar sk_plr_dmg_grenade( "sk_plr_dmg_grenade", "0" );
#endif

ConVar ammo_max( "ammo_max", "5000", FCVAR_REPLICATED );

#ifdef GAME_DLL
// --------------------------------------------------------------------------------------------------- //
// Voice helper
// --------------------------------------------------------------------------------------------------- //
class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
	{
		// Dead players can only be heard by other dead team mates but only if a match is in progress
		if ( TFCSGameRules()->State_Get() != GR_STATE_TEAM_WIN && TFCSGameRules()->State_Get() != GR_STATE_GAME_OVER ) 
		{
			if ( pTalker->IsAlive() == false )
			{
				if ( pListener->IsAlive() == false )
					return ( pListener->InSameTeam( pTalker ) );
				return false;
			}
		}
		return ( pListener->InSameTeam( pTalker ) );
	}
};
CVoiceGameMgrHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;
#endif

CTFCSGameRules::CTFCSGameRules()
{
#ifdef GAME_DLL
	//TODO: Team managers

#endif
	//TODO: Prematch stuff

}

CTFCSGameRules::~CTFCSGameRules()
{
	
}

bool CTFCSGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 );
}

#ifdef GAME_DLL
void CTFCSGameRules::Think()
{
	BaseClass::Think();
}

Vector DropToGround(
	CBaseEntity *pMainEnt,
	const Vector &vPos,
	const Vector &vMins,
	const Vector &vMaxs )
{
	trace_t trace;
	UTIL_TraceHull( vPos, vPos + Vector( 0, 0, -500 ), vMins, vMaxs, MASK_SOLID, pMainEnt, COLLISION_GROUP_NONE, &trace );
	return trace.endpos;
}

CBaseEntity *CTFCSGameRules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	// get valid spawn point
	CBaseEntity *pSpawnSpot = pPlayer->EntSelectSpawnPoint();

	// drop down to ground
	Vector GroundPos = DropToGround( pPlayer, pSpawnSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX );

	// Move the player to the place it said.
	pPlayer->Teleport( &GroundPos, &pSpawnSpot->GetLocalAngles(), &vec3_origin );
	pPlayer->m_Local.m_vecPunchAngle = vec3_angle;

	return pSpawnSpot;
}

bool CTFCSGameRules::IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayer *pPlayer )
{
	if ( !pSpot->IsTriggered( pPlayer ) )
		return false;

	// Check if it is disabled by Enable/Disable
	CSpawnPoint *pSpawnPoint = dynamic_cast< CSpawnPoint * >( pSpot );
	if ( pSpawnPoint )
	{
		if ( pSpawnPoint->IsDisabled() )
			return false;
	}

	Vector mins = GetViewVectors()->m_vHullMin;
	Vector maxs = GetViewVectors()->m_vHullMax;

	Vector vTestMins = pSpot->GetAbsOrigin() + mins;
	Vector vTestMaxs = pSpot->GetAbsOrigin() + maxs;

	// First test the starting origin.
	return UTIL_IsSpaceEmpty( pPlayer, vTestMins, vTestMaxs );
}

void CTFCSGameRules::PlayerSpawn( CBasePlayer *p )
{
	//TODO: Finish this when player class is completed
	BaseClass::PlayerSpawn( p );
}

void CTFCSGameRules::RadiusDamage(const CTakeDamageInfo &info, const Vector &vecSrc, float flRadius, int iClassIgnore, CBaseEntity *pEntityIgnore)
{
	BaseClass::RadiusDamage( info, vecSrc, flRadius, iClassIgnore, pEntityIgnore );
}

bool CTFCSGameRules::ClientCommand( CBaseEntity *pEdict, const CCommand &args )
{
	return BaseClass::ClientCommand( pEdict, args );
}

const char *CTFCSGameRules::GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer )
{
	return "";
}
#endif
//-----------------------------------------------------------------------------
// Purpose: Init CS ammo definitions
//-----------------------------------------------------------------------------

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB( grains )	( 0.002285*( grains )/16.0f )
#define BULLET_MASS_GRAINS_TO_KG( grains )	lbs2kg( BULLET_MASS_GRAINS_TO_LB( grains ) )

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			1	

// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE( grains, ftpersec )	( ( ftpersec )*12*BULLET_MASS_GRAINS_TO_KG( grains )*BULLET_IMPULSE_EXAGGERATION )

CAmmoDef* GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;

	if ( !bInitted )
	{
		bInitted = true;

		for ( int i = 1; i < AMMO_LAST; i++ )
			def.AddAmmoType( g_aAmmoNames[i], DMG_BULLET, TRACER_NONE, 0, 0, 200, 1, 0 );
	}

	return &def;
}
//#endif