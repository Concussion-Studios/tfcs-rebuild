#include "cbase.h"
#include "tfcs_gamerules.h"
#include "tfcs_weapon_base.h"
#include "tfcs_playerclass_parse.h"
#include "KeyValues.h"
#include "ammodef.h"

#ifdef CLIENT_DLL
	#include <game/client/iviewport.h>
	#include "c_tfcs_player.h"
	#include "c_tfcs_objective_resource.h"
#else
	#include "basemultiplayerplayer.h"
	#include "voice_gamemgr.h"
	#include "items.h"
	#include "team.h"
	#include "tfcs_player.h"
	#include "tfcs_team.h"
	#include "player_resource.h"
	#include "team_spawnpoint.h"
	#include "filesystem.h"
	#include "tfcs_objective_resource.h"
	#include "tfcs_player_resource.h"
	#include "team_control_point_master.h"
	#include "team_control_point_master.h"
	#include "coordsize.h"
	//#include "entity_capture_flag.h"
	#include "tfcs_player_resource.h"
	#include "tier0/icommandline.h"
	#include "activitylist.h"
	#include "AI_ResponseSystem.h"
	#include "hltvdirector.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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
	virtual bool CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
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
	ConColorMsg( Color( 86, 156, 143, 255 ), "[CTFCSGameRules] Creating Gamerules....\n" );

	// Create teams.
	TFCSTeamMgr()->Init();

	if ( filesystem->FileExists( UTIL_VarArgs( "maps/cfg/%s.cfg", STRING( gpGlobals->mapname ) ) ) )
	{
		// Execute a map specific cfg file
		ConColorMsg( Color( 86, 156, 143, 255 ), "[CTFCSGameRules] Executing map %s config file\n", STRING( gpGlobals->mapname ) );
		engine->ServerCommand( UTIL_VarArgs( "exec %s.cfg */maps\n", STRING( gpGlobals->mapname ) ) );
		engine->ServerExecute();
	}
	else
	{
		ConColorMsg( Color( 86, 156, 143, 255 ), "[CTFCSGameRules] Could not load map %s config file skiping...\n", STRING( gpGlobals->mapname ) );
	}
#else
	ConColorMsg( Color( 86, 156, 143, 255 ), "[C_TFCSGameRules] Creating Gamerules....\n" );
#endif

	//TODO: Prematch stuff
	InitPlayerClasses();
}

CTFCSGameRules::~CTFCSGameRules()
{
#ifndef CLIENT_DLL
	ConColorMsg( Color( 86, 156, 143, 255 ), "[CTFCSGameRules] Destroying Gamerules....\n" );

	// Note, don't delete each team since they are in the gEntList and will 
	// automatically be deleted from there, instead.
	TFCSTeamMgr()->Shutdown();
#else
	ConColorMsg( Color( 86, 156, 143, 255 ), "[C_TFCSGameRules] Destroying Gamerules....\n" );
#endif	
}

#ifdef GAME_DLL
void CTFCSGameRules::Think()
{
	BaseClass::Think();
}

void CTFCSGameRules::CreateStandardEntities()
{
	// Create the player resource
	g_pPlayerResource = (CPlayerResource*)CBaseEntity::Create( "tfcs_player_manager", vec3_origin, vec3_angle );

	// Create the objective resource
	g_pObjectiveResource = (CTFCSObjectiveResource *)CBaseEntity::Create( "tfcs_objective_resource", vec3_origin, vec3_angle );

	Assert( g_pObjectiveResource );

	// Create the entity that will send our data to the client.
	CBaseEntity *pEnt = CBaseEntity::Create( "tfcs_gamerules", vec3_origin, vec3_angle );
	Assert( pEnt );
	pEnt->SetName( AllocPooledString( "tfcs_gamerules" ) );
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

 void TestSpawnPointType( const char *pEntClassName )
{
	// Find the next spawn spot.
	CBaseEntity *pSpot = gEntList.FindEntityByClassname( NULL, pEntClassName );

	while( pSpot )
	{
		// trace a box here
		Vector vTestMins = pSpot->GetAbsOrigin() + VEC_HULL_MIN;
		Vector vTestMaxs = pSpot->GetAbsOrigin() + VEC_HULL_MAX;

		if ( UTIL_IsSpaceEmpty( pSpot, vTestMins, vTestMaxs ) )
		{
			// the successful spawn point's location
			NDebugOverlay::Box( pSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 0, 255, 0, 100, 60 );

			// drop down to ground
			Vector GroundPos = DropToGround( NULL, pSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX );

			// the location the player will spawn at
			NDebugOverlay::Box( GroundPos, VEC_HULL_MIN, VEC_HULL_MAX, 0, 0, 255, 100, 60 );

			// draw the spawn angles
			QAngle spotAngles = pSpot->GetLocalAngles();
			Vector vecForward;
			AngleVectors( spotAngles, &vecForward );
			NDebugOverlay::HorzArrow( pSpot->GetAbsOrigin(), pSpot->GetAbsOrigin() + vecForward * 32, 10, 255, 0, 0, 255, true, 60 );
		}
		else
		{
			// failed spawn point location
			NDebugOverlay::Box( pSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 255, 0, 0, 100, 60 );
		}

		// increment pSpot
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	}
}

void TestSpawns()
{
	TestSpawnPointType( "info_player_teamspawn" );
}
ConCommand cc_TestSpawns( "map_showspawnpoints", TestSpawns, "Dev - test the spawn points, draws for 60 seconds", FCVAR_CHEAT );

CBaseEntity *CTFCSGameRules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	// get valid spawn point
	CBaseEntity *pSpawnSpot = pPlayer->EntSelectSpawnPoint();

	// drop down to ground
	Vector GroundPos = DropToGround( pPlayer, pSpawnSpot->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX );

	// Move the player to the place it said.
	pPlayer->SetLocalOrigin( GroundPos + Vector(0,0,1) );
	pPlayer->SetAbsVelocity( vec3_origin );
	pPlayer->SetLocalAngles( pSpawnSpot->GetLocalAngles() );
	pPlayer->m_Local.m_vecPunchAngle = vec3_angle;
	pPlayer->m_Local.m_vecPunchAngleVel = vec3_angle;
	pPlayer->SnapEyeAngles( pSpawnSpot->GetLocalAngles() );

	return pSpawnSpot;
}

bool CTFCSGameRules::IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayer *pPlayer, bool bIgnorePlayers )
{
	if ( !pSpot->IsTriggered( pPlayer ) )
		return false;

	// Check if it is disabled by Enable/Disable
	CTeamSpawnPoint *pSpawnPoint = dynamic_cast< CTeamSpawnPoint* >( pSpot );
	if ( pSpawnPoint )
	{
		if ( pSpawnPoint->IsDisabled() )
			return false;
	}

	Vector mins = GetViewVectors()->m_vHullMin;
	Vector maxs = GetViewVectors()->m_vHullMax;

	if ( !bIgnorePlayers )
	{
		Vector vTestMins = pSpot->GetAbsOrigin() + mins;
		Vector vTestMaxs = pSpot->GetAbsOrigin() + maxs;
		return UTIL_IsSpaceEmpty( pPlayer, vTestMins, vTestMaxs );
	}

	trace_t trace;
	UTIL_TraceHull( pSpot->GetAbsOrigin(), pSpot->GetAbsOrigin(), mins, maxs, MASK_PLAYERSOLID, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );
	return ( trace.fraction == 1 && trace.allsolid != 1 && (trace.startsolid != 1) );
}

void CTFCSGameRules::PlayerSpawn( CBasePlayer *p )
{
	//TODO: Finish this when player class is completed
	p->Spawn();
	BaseClass::PlayerSpawn( p );
}

void CTFCSGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrc, float flRadius, int iClassIgnore, CBaseEntity *pEntityIgnore )
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

bool CTFCSGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	// swap so that lowest is always first
	if ( collisionGroup0 > collisionGroup1 )
		V_swap( collisionGroup0, collisionGroup1 );
	
	//Don't stand on COLLISION_GROUP_WEAPONs
	if( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT &&
		collisionGroup1 == COLLISION_GROUP_WEAPON )
		return false;

	// Don't stand on projectiles
	if( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT &&
		collisionGroup1 == COLLISION_GROUP_PROJECTILE )
		return false;

	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 ); 
}

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