//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
//#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "tfcs_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "tfcs_player_shared.h"
#include "predicted_viewmodel.h"
#include "in_buttons.h"
#include "tfcs_gamerules.h"
#include "tfcs_player_shared.h"
#include "tfcs_playerclass_parse.h"
#include "KeyValues.h"
#include "team.h"
//#include "weapon_hl2mpbase.h"
#include "entity_spawnpoint.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "tier0/vprof.h"
#include "bone_setup.h"
#include "datacache/imdlcache.h"
#include "obstacle_pushaway.h"
#include "ilagcompensationmanager.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "GameStats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern int	gEvilImpulse101;

#define TFCS_PLAYER_PHYSDAMAGE_SCALE 4.0f
#define TFCS_PUSHAWAY_THINK_CONTEXT	"TFCSPushawayThink"
#define CYCLELATCH_UPDATE_INTERVAL	0.2f

#define TEAM_CHANGE_INTERVAL 5.0f

#pragma warning( disable : 4355 )
#pragma warning( disable : 4189 )

EHANDLE g_pLastSpawnPoints[ TEAM_COUNT ];

ConVar tfcs_showstatetransitions( "tfcs_ShowStateTransitions", "-2", FCVAR_CHEAT, "tfcs_ShowStateTransitions <ent index or -1 for all>. Show player state transitions." );


//* **************** CTEPlayerAnimEvent* *********************

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nData ), 32 )
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( (const Vector&)pPlayer->EyePosition() );

	//Tony; use prediction rules.
	filter.UsePredictionRules();
	
	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.m_nData = nData;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

//* **************** CTFCSRagdoll* *********************

LINK_ENTITY_TO_CLASS( tfcs_ragdoll, CTFCSRagdoll );

IMPLEMENT_SERVERCLASS_ST( CTFCSRagdoll, DT_TFCSRagdoll )
	SendPropVector( SENDINFO( m_vecRagdollOrigin ), -1, SPROP_COORD ),
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
	SendPropInt( SENDINFO( m_nForceBone), 8, 0 ),
	SendPropVector( SENDINFO( m_vecForce), -1, SPROP_NOSCALE ),
	SendPropVector( SENDINFO( m_vecRagdollVelocity ), 13, SPROP_ROUNDDOWN, -2048.0f, 2048.0f ),
	SendPropBool( SENDINFO( m_bGib ) ),
	SendPropBool( SENDINFO( m_bBurning ) ),
	SendPropExclude( "DT_BaseEntity", "m_nRenderFX" ),
END_SEND_TABLE()

//* **************** CTFCSPlayer* *********************

LINK_ENTITY_TO_CLASS( player, CTFCSPlayer );

extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );

//Tony; this should ideally be added to dt_send.cpp
void* SendProxy_SendNonLocalDataTable( const SendProp *pProp, const void *pStruct, const void *pVarData, CSendProxyRecipients *pRecipients, int objectID )
{
	pRecipients->SetAllRecipients();
	pRecipients->ClearRecipient( objectID - 1 );
	return ( void * )pVarData;
}
REGISTER_SEND_PROXY_NON_MODIFIED_POINTER( SendProxy_SendNonLocalDataTable );

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSLocalPlayerExclusive )
	// send a hi-res origin to the local player for use in prediction
	SendPropVector	(SENDINFO(m_vecOrigin), -1,  SPROP_NOSCALE|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),

	SendPropFloat( SENDINFO( m_ArmorClass ) ),
	//SendPropInt( SENDINFO( m_Armor ), 8, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_MaxArmor ), 8, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flConcussTime ) ),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
	// send a lo-res origin to other players
	SendPropVector	(SENDINFO(m_vecOrigin), -1,  SPROP_COORD_MP_LOWPRECISION|SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropFloat( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN ),
	// Only need to latch cycle for other players
	// If you increase the number of bits networked, make sure to also modify the code below and in the client class.
	SendPropInt( SENDINFO( m_cycleLatch ), 4, SPROP_UNSIGNED ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST(CTFCSPlayer, DT_TFCSPlayer)
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),

	SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),

	// playeranimstate and clientside animation takes care of these on the client
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

	SendPropExclude( "DT_BaseFlex", "m_flexWeight" ),
	SendPropExclude( "DT_BaseFlex", "m_blinktoggle" ),
	SendPropExclude( "DT_BaseFlex", "m_viewtarget" ),

	// Other players' water level is networked for animations.
	SendPropInt( SENDINFO( m_nWaterLevel ), 2, SPROP_UNSIGNED ),
	SendPropExclude( "DT_LocalPlayerExclusive", "m_nWaterLevel" ),

	// Data that only gets sent to the local player.
	SendPropDataTable( SENDINFO_DT( m_Shared ), &REFERENCE_SEND_TABLE( DT_TFCSPlayerShared ) ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "tfcs_localdata", 0, &REFERENCE_SEND_TABLE(DT_TFCSLocalPlayerExclusive), SendProxy_SendLocalDataTable ),
	// Data that gets sent to all other players
	SendPropDataTable( "tfcs_nonlocaldata", 0, &REFERENCE_SEND_TABLE(DT_TFCSNonLocalPlayerExclusive), SendProxy_SendNonLocalDataTable ),

	SendPropEHandle( SENDINFO( m_hRagdoll ) ),
	SendPropInt( SENDINFO( m_iSpawnInterpCounter), 4 ),
		
END_SEND_TABLE()

BEGIN_DATADESC( CTFCSPlayer )
	DEFINE_FIELD( m_ArmorClass, FIELD_FLOAT ),
	//DEFINE_FIELD( m_ArmorValue, FIELD_INTEGER ),
	DEFINE_FIELD( m_MaxArmor, FIELD_INTEGER ),
	DEFINE_FIELD( m_flConcussTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_flCrippleTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_iCrippleLevel, FIELD_INTEGER ),
END_DATADESC()

CTFCSPlayer::CTFCSPlayer()
{
	//Tony; create our player animation state.
	m_PlayerAnimState = CreateTFCSPlayerAnimState( this );
	UseClientSideAnimation();

	m_angEyeAngles.Init();

	SetPredictionEligible( true );

	m_iLastWeaponFireUsercmd = 0;

	m_flNextTeamChangeTime = 0.0f;

	m_iSpawnInterpCounter = 0;

	m_lifeState = LIFE_DEAD; // Start "dead".

	m_Shared.Init( this );

	m_cycleLatch = 0;
	m_cycleLatchTimer.Invalidate();

	//SetViewOffset( TFC_PLAYER_VIEW_OFFSET );

	SetContextThink( &CTFCSPlayer::TFCSPlayerThink, gpGlobals->curtime, "TFCSPlayerThink" );
}

CTFCSPlayer::~CTFCSPlayer( void )
{
	if ( GetAnimState() )
		GetAnimState()->Release();
}

void CTFCSPlayer::UpdateOnRemove( void )
{
	RemoveRagdollEntity();

	BaseClass::UpdateOnRemove();
}

CTFCSPlayer *CTFCSPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CTFCSPlayer::s_PlayerEdict = ed;
	return (CTFCSPlayer*)CreateEntityByName( className );
}

void CTFCSPlayer::TFCSPlayerThink()
{
	SetContextThink( &CTFCSPlayer::TFCSPlayerThink, gpGlobals->curtime, "TFCSPlayerThink" );
}

void CTFCSPlayer::Precache( void )
{
	BaseClass::Precache();

	PrecacheModel ( "sprites/glow01.vmt" );

	// Precache player models
	for ( int i = 0; i < CLASS_LAST; i++ )
	{
		const char *pszModel = GetClassData( i )->m_szModelName;
		if ( pszModel && pszModel[0] )
		{
			PrecacheModel( pszModel );
		}
	}

	// Precache arm models
	for ( int i = 0; i < CLASS_LAST; i++ )
	{
		const char *pszHandModel = GetClassData( i )->m_szArmsModel;
		if ( pszHandModel && pszHandModel[0] )
		{
			PrecacheModel( pszHandModel );
		}
	}
}

void CTFCSPlayer::GiveAllItems( void )
{
}

// Set the player up with the default weapons, ammo, etc.
void CTFCSPlayer::GiveDefaultItems( void )
{
	// Get the player class data.
	TFCSPlayerClassInfo_t *data = GetClassData( m_Shared.GetClassIndex() );	
	
	//Get rid of all ammo and weapons first
	RemoveAllAmmo();
	RemoveAllWeapons();

	//Give ammo
	for ( int iAmmo = AMMO_DUMMY; iAmmo < AMMO_LAST; ++iAmmo )
		GiveAmmo( data->m_aSpawnAmmo[iAmmo], iAmmo );

	//Give weapons
	for ( int iSlot = 0; iSlot < TFCS_MAX_WEAPON_SLOTS; ++iSlot )
	{
		if ( data->m_aWeapons[iSlot] != 0 )
		{
			const char *pszWeaponName = WeaponIDToAlias( data->m_aWeapons[iSlot] );
			CTFCSWeaponBase *pWpn = ( CTFCSWeaponBase* )GiveNamedItem( pszWeaponName );
		}
	}
}

//Give players their spawn items
void CTFCSPlayer::InitClass( void )
{
	TFCSPlayerClassInfo_t *data = GetClassData( /*CLASS_SCOUT*/ m_Shared.GetClassIndex() );
	int iMaxHealth = data->m_iMaxHealth;

	//Give health and armor
	SetMaxHealth( iMaxHealth );
	SetHealth( iMaxHealth );

	m_MaxArmor = data->m_iMaxArmor;
	m_ArmorValue = data->m_iSpawnArmor;

	SetArmorClass( data->m_flArmorClass );

	//Set max speed
	SetMaxSpeed( data->m_flMaxSpeed );


	// Give default items for class.
	GiveDefaultItems();

	//Give ammo
	/*for ( int iAmmo = AMMO_DUMMY; iAmmo < AMMO_LAST; ++iAmmo )
	{
		GiveAmmo( data->m_aSpawnAmmo[iAmmo], iAmmo );
	}*/

	//Give weapons
	/*for ( int iSlot = 0; iSlot < TFCS_MAX_WEAPON_SLOTS; ++iSlot )
	{
		if ( data->m_aWeapons[iSlot] != 0 )
		{
			const char *pszWeaponName = WeaponIDToAlias( data->m_aWeapons[iSlot] );
			CTFCSWeaponBase *pWpn = (CTFCSWeaponBase *)GiveNamedItem( pszWeaponName );
		}
	}*/
}

void CTFCSPlayer::PickDefaultSpawnTeam( void )
{
	if ( GetTeamNumber() == 0 )
	{
		if ( TFCSGameRules()->IsTeamplay() == false )
			ChangeTeam( TEAM_UNASSIGNED );
		else
			ChangeTeam( TEAM_SPECTATOR );
	}
}

void CTFCSPlayer::TFCSPushawayThink( void )
{
	// Push physics props out of our way.
	PerformObstaclePushaway( this );
	SetNextThink( gpGlobals->curtime + PUSHAWAY_THINK_INTERVAL, TFCS_PUSHAWAY_THINK_CONTEXT );
}

void CTFCSPlayer::ForceRespawn()
{
	BaseClass::ForceRespawn();
}

void CTFCSPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn();
	State_Enter( STATE_WELCOME );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSPlayer::Spawn( void )
{
	MDLCACHE_CRITICAL_SECTION();

	SetModel( GetClassData( m_Shared.GetClassIndex() )->m_szModelName );

	m_flNextTeamChangeTime = 0.0f;

	PickDefaultSpawnTeam();

	// Collision group must be set before base class spawn.
	// Spawnpoint look-up depend on the fact that player's are solid which is done before setting player's collision group...
	SetCollisionGroup( COLLISION_GROUP_PLAYER );

	BaseClass::Spawn();

	if ( !m_Local.m_PlayerFog.m_hCtrl.Get() )
		m_Local.m_skybox3d.fog.enable = false;
	
	if ( !IsObserver() )
	{
		pl.deadflag = false;

		SetMoveType( MOVETYPE_WALK );
		RemoveSolidFlags( FSOLID_NOT_SOLID );

		RemoveEffects( EF_NODRAW );
		
		GiveDefaultItems();
		InitClass();
	}

	RemoveEffects( EF_NOINTERP );

	m_nRenderFX = kRenderNormal;

	m_Local.m_iHideHUD = 0;
	
	AddFlag( FL_ONGROUND ); // set the player on the ground at the start of the round.

	m_impactEnergyScale = TFCS_PLAYER_PHYSDAMAGE_SCALE;

	// Don't display crosshair if the map is a background.
	if ( gpGlobals->eLoadType == MapLoad_Background )
		ShowCrosshair( false );

	if ( TFCSGameRules()->IsIntermission() )
		AddFlag( FL_FROZEN );
	else
		RemoveFlag( FL_FROZEN );

	m_iSpawnInterpCounter = (m_iSpawnInterpCounter + 1) % 8;

	if ( GetAnimState() )
		GetAnimState()->ClearAnimationState();

	m_nSkin = GetTeamNumber();

	RemoveRagdollEntity();

	m_cycleLatchTimer.Start( CYCLELATCH_UPDATE_INTERVAL );

	//Tony; do the spawn animevent
	DoAnimationEvent( PLAYERANIMEVENT_SPAWN );

	Vector mins = VEC_HULL_MIN;
	Vector maxs = VEC_HULL_MAX;
	CollisionProp()->SetSurroundingBoundsType( USE_SPECIFIED_BOUNDS, &mins, &maxs );

	SetContextThink( &CTFCSPlayer::TFCSPushawayThink, gpGlobals->curtime + PUSHAWAY_THINK_INTERVAL, TFCS_PUSHAWAY_THINK_CONTEXT );
}

bool CTFCSPlayer::Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex )
{
	bool bRet = BaseClass::Weapon_Switch( pWeapon, viewmodelindex );
	return bRet;
}

void CTFCSPlayer::PreThink( void )
{
	//Andrew; See http://forums.steampowered.com/forums/showthread.php?t=1372727
	QAngle vOldAngles = GetLocalAngles();
	QAngle vTempAngles = GetLocalAngles();

	vTempAngles = EyeAngles();

	if ( vTempAngles[PITCH] > 180.0f )
		vTempAngles[PITCH] -= 360.0f;

	SetLocalAngles( vTempAngles );

	BaseClass::PreThink();

	//Reset bullet force accumulator, only lasts one frame
	m_vecTotalBulletForce = vec3_origin;

	SetLocalAngles( vOldAngles );
}

void CTFCSPlayer::Think()
{
	BaseClass::Think();
}


void CTFCSPlayer::PostThink( void )
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	if ( GetAnimState() )
		GetAnimState()->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

	if ( IsAlive() && m_cycleLatchTimer.IsElapsed() )
	{
		m_cycleLatchTimer.Start( CYCLELATCH_UPDATE_INTERVAL );
		// Compress the cycle into 4 bits. Can represent 0.0625 in steps which is enough.
		m_cycleLatch.GetForModify() = 16 * GetCycle();
	}
}

void CTFCSPlayer::FireBullets ( const FireBulletsInfo_t &info )
{
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( this, this->GetCurrentCommand() );

	FireBulletsInfo_t modinfo = info;

	/*CTFCSWeaponBase* pWeapon = dynamic_cast<CTFCSWeaponBase *>( GetActiveWeapon() );
	if ( pWeapon )
		modinfo.m_iPlayerDamage = modinfo.m_flDamage = pWeapon->GetTFCSWpnData().m_iPlayerDamage;*/

	NoteWeaponFired();

	BaseClass::FireBullets( modinfo );

	// Move other players back to history positions based on local player's lag
	lagcompensation->FinishLagCompensation( this );
}

void CTFCSPlayer::NoteWeaponFired( void )
{
	Assert( m_pCurrentCommand );
	if( m_pCurrentCommand )
		m_iLastWeaponFireUsercmd = m_pCurrentCommand->command_number;
}

bool CTFCSPlayer::WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const
{
	// No need to lag compensate at all if we're not attacking in this command and
	// we haven't attacked recently.
	if ( !( pCmd->buttons & IN_ATTACK ) && (pCmd->command_number - m_iLastWeaponFireUsercmd > 5) )
		return false;

	return BaseClass::WantsLagCompensationOnEntity( pPlayer, pCmd, pEntityTransmitBits );
}

//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CTFCSPlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

	// Can I have this weapon type?
	if ( !IsAllowedToPickupWeapons() )
		return false;

	if ( pOwner || !Weapon_CanUse( pWeapon ) || !g_pGameRules->CanHavePlayerItem( this, pWeapon ) )
	{
		if ( gEvilImpulse101 )
			UTIL_Remove( pWeapon );

		return false;
	}

	// Don't let the player fetch weapons through walls (use MASK_SOLID so that you can't pickup through windows)
	if( !pWeapon->FVisible( this, MASK_SOLID ) && !(GetFlags() & FL_NOTARGET) )
		return false;

	bool bOwnsWeaponAlready = !!Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType());

	if ( bOwnsWeaponAlready == true ) 
	{
		//If we have room for the ammo, then "take" the weapon too.
		 if ( Weapon_EquipAmmoOnly( pWeapon ) )
		 {
			 pWeapon->CheckRespawn();

			 UTIL_Remove( pWeapon );
			 return true;
		 }
		 else
			 return false;
	}

	pWeapon->CheckRespawn();
	Weapon_Equip( pWeapon );

	return true;
}

void CTFCSPlayer::ChangeTeam( int iTeam )
{
/*	if ( GetNextTeamChangeTime() >= gpGlobals->curtime )
	{
		char szReturnString[128];
		Q_snprintf( szReturnString, sizeof( szReturnString ), "Please wait %d more seconds before trying to switch teams again.\n", (int)(GetNextTeamChangeTime() - gpGlobals->curtime) );

		ClientPrint( this, HUD_PRINTTALK, szReturnString );
		return;
	}*/

	bool bKill = false;

	if ( TFCSGameRules()->IsTeamplay() != true && iTeam != TEAM_SPECTATOR )
	{
		//don't let them try to join combine or rebels during deathmatch.
		iTeam = TEAM_UNASSIGNED;
	}

	if ( TFCSGameRules()->IsTeamplay() == true )
	{
		if ( iTeam != GetTeamNumber() && GetTeamNumber() != TEAM_UNASSIGNED )
		{
			bKill = true;
		}
	}

	BaseClass::ChangeTeam( iTeam );

	m_flNextTeamChangeTime = gpGlobals->curtime + TEAM_CHANGE_INTERVAL;

	if ( iTeam == TEAM_SPECTATOR )
	{
		RemoveAllItems( true );

		int observerMode = m_iObserverLastMode;
		if ( IsNetClient() )
		{
			const char *pIdealMode = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ), "cl_spec_mode" );
			if ( pIdealMode )
			{
				observerMode = atoi( pIdealMode );
				if ( observerMode <= OBS_MODE_FIXED || observerMode > OBS_MODE_ROAMING )
					observerMode = m_iObserverLastMode;
			}
		}

		StartObserverMode( observerMode );
	}

	if ( bKill == true )
		CommitSuicide();
}

void CTFCSPlayer::ShowClassSelectMenu()
{
	if ( GetTeamNumber() == TEAM_BLUE )
		ShowViewPortPanel( PANEL_CLASS_BLUE );
	else if ( GetTeamNumber() == TEAM_RED )
		ShowViewPortPanel( PANEL_CLASS_RED );
	else if ( GetTeamNumber() == TEAM_YELLOW )
		ShowViewPortPanel( PANEL_CLASS_YELLOW );
	else if ( GetTeamNumber() == TEAM_GREEN )
		ShowViewPortPanel( PANEL_CLASS_GREEN );
}

bool CTFCSPlayer::HandleCommand_JoinClass( int iClass )
{
	Assert( GetTeamNumber() != TEAM_SPECTATOR );
	Assert( GetTeamNumber() != TEAM_UNASSIGNED );

	if ( GetTeamNumber() == TEAM_SPECTATOR )
	{
		return false;
	}

	if ( iClass == CLASS_LAST )
	{
		return false;
	}

	int iOldPlayerClass = m_Shared.DesiredPlayerClass();
	const char *classname = g_aClassNames_NonLocalized[iClass];

	// Trying to join current class
	if ( iClass == iOldPlayerClass )
	{
		return true;
	}

	//TODO: Determine if certain classes are allowed on certain gamemodes and maps through tfcs_gamerules

	//Random class
	if ( iClass == CLASS_RANDOM )
	{
		if ( IsAlive() )
			ClientPrint( this, HUD_PRINTTALK, "#game_respawn_asrandom" );
		else
			ClientPrint( this, HUD_PRINTTALK, "#game_spawn_asrandom" );
	}
	else
	{
		if ( IsAlive() )
			ClientPrint( this, HUD_PRINTTALK, "#game_respawn_as", classname );
		else
			ClientPrint( this, HUD_PRINTTALK, "#game_spawn_as", classname );
	}

	//Create a game event
	IGameEvent *event = gameeventmanager->CreateEvent( "player_changeclass" );
	if ( event )
	{
		event->SetInt( "userid", GetUserID() );
		event->SetInt( "class", iClass );
	}

	//Kill the player so that they join class (gives timed respawn penalty)
	//TODO: Toggle suicide on choose playerclass in settings/convar
	CommitSuicide();

	return true;
}

bool CTFCSPlayer::HandleCommand_JoinTeam( int team )
{
	if ( !GetGlobalTeam( team ) || team == 0 )
	{
		Warning( "HandleCommand_JoinTeam( %d ) - invalid team index.\n", team );
		return false;
	}

	if ( team == TEAM_SPECTATOR )
	{
		// Prevent this is the cvar is set
		if ( !mp_allowspectators.GetInt() )
		{
			ClientPrint( this, HUD_PRINTCENTER, "#Cannot_Be_Spectator" );
			return false;
		}

		if ( GetTeamNumber() != TEAM_UNASSIGNED && !IsDead() )
		{
			m_fNextSuicideTime = gpGlobals->curtime;	// allow the suicide to work

			CommitSuicide();

			// add 1 to frags to balance out the 1 subtracted for killing yourself
			IncrementFragCount( 1 );
		}

		ChangeTeam( TEAM_SPECTATOR );

		return true;
	}
	else
	{
		StopObserverMode();

		SetMoveType( MOVETYPE_WALK );
	
		// md 8/15/07 - They'll get set back to solid when they actually respawn. If we set them solid now and mp_forcerespawn
		// is false, then they'll be spectating but blocking live players from moving.
		// RemoveSolidFlags( FSOLID_NOT_SOLID );
	
		m_Local.m_iHideHUD = 0;
	}

	// Switch their actual team...
	ChangeTeam( team );

	return true;
}

bool CTFCSPlayer::ClientCommand( const CCommand &args )
{
	if ( FStrEq( args[0], "spectate" ) )
	{
		if ( ShouldRunRateLimitedCommand( args ) )
		{
			// instantly join spectators
			HandleCommand_JoinTeam( TEAM_SPECTATOR );	
		}
		return true;
	}
	else if ( FStrEq( args[0], "joinclass" ) )
	{
		if  ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad joinclass sntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			int iClass = atoi( args[1] );
			HandleCommand_JoinClass( iClass );
		}
	}
	else if ( FStrEq( args[0], "jointeam" ) ) 
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad jointeam syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			int iTeam = atoi( args[1] );
			HandleCommand_JoinTeam( iTeam );
		}
		return true;
	}
	else if ( FStrEq( args[0], "joingame" ) )
	{
		return true;
	}

	return BaseClass::ClientCommand( args );
}

void CTFCSPlayer::CheatImpulseCommands( int iImpulse )
{
	switch ( iImpulse )
	{
		case 101:
			{
				if( sv_cheats->GetBool() )
				{
					GiveAllItems();
				}
			}
			break;

		default:
			BaseClass::CheatImpulseCommands( iImpulse );
	}
}

void CTFCSPlayer::CommitSuicide( bool bExplode /* = false */, bool bForce /*= false*/ )
{
	//m_iSuicideCustomKillFlags = DMG_CUSTOM_SUICIDE;

	BaseClass::CommitSuicide( bExplode, bForce );
}

void CTFCSPlayer::CreateViewModel( int index /*=0*/ )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

void CTFCSPlayer::CreateRagdollEntity( void )
{
	CreateRagdollEntity( false, false );
}

void CTFCSPlayer::CreateRagdollEntity( bool bGib, bool bBurning )
{
	RemoveRagdollEntity();

	// If we already have a ragdoll, don't make another one.
	CTFCSRagdoll* pRagdoll = dynamic_cast< CTFCSRagdoll* >( m_hRagdoll.Get() );

	// create a new one
	if ( !pRagdoll )
		pRagdoll = dynamic_cast<CTFCSRagdoll*>( CreateEntityByName( "tfcs_ragdoll" ) );

	if ( pRagdoll )
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->m_vecForce = m_vecForce;
		pRagdoll->m_bGib = bGib;
		pRagdoll->m_bBurning = bBurning;
		pRagdoll->SetAbsOrigin( GetAbsOrigin() );
	}

	//AddEffects(EF_NODRAW | EF_NOSHADOW);

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}

void CTFCSPlayer::RemoveRagdollEntity()
{
	if ( m_hRagdoll )
	{
		UTIL_RemoveImmediate( m_hRagdoll );
		m_hRagdoll = NULL;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CTFCSPlayer::FlashlightIsOn( void )
{
	return IsEffectActive( EF_DIMLIGHT );
}

extern ConVar flashlight;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CTFCSPlayer::FlashlightTurnOn( void )
{
	BaseClass::FlashlightTurnOn();

	if( flashlight.GetInt() > 0 && IsAlive() )
	{
		AddEffects( EF_DIMLIGHT );
		EmitSound( "Player.FlashlightOn" );
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CTFCSPlayer::FlashlightTurnOff( void )
{
	BaseClass::FlashlightTurnOff();

	RemoveEffects( EF_DIMLIGHT );
	
	if( IsAlive() )
		EmitSound( "Player.FlashlightOff" );
}

void CTFCSPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	//update damage info with our accumulated physics force
	CTakeDamageInfo subinfo = info;
	subinfo.SetDamageForce( m_vecTotalBulletForce );

	DoAnimationEvent( PLAYERANIMEVENT_DIE );

	bool bBurning = ( ( info.GetDamageType() & ( DMG_BURN | DMG_BLAST | DMG_PLASMA | DMG_ENERGYBEAM ) ) != 0 );
	bool bGib = ShouldGib(info);

	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.
	CreateRagdollEntity( bGib, bBurning );

	// ...and employ a minor hack to stop CBaseCombatCharacter creating its own
	const_cast< CTakeDamageInfo* >( &info )->AddDamageType( DMG_REMOVENORAGDOLL );

	// We can't be on fire while dead!
	Extinguish();

	CBaseEntity* pEffect = GetEffectEntity();
	if ( pEffect != nullptr )
	{
		UTIL_Remove( pEffect );
		SetEffectEntity( nullptr );
	}

	// show killer in death cam mode
	if ( info.GetAttacker() && ( info.GetAttacker()->IsPlayer() || info.GetAttacker()->IsNPC() ) && info.GetAttacker() != ( CBaseEntity* )this )
	{
		m_hObserverTarget.Set( info.GetAttacker() );
		SetFOV( this, 0 ); // reset
	}
	else
		m_hObserverTarget.Set( NULL );

	BaseClass::Event_Killed( subinfo );

	if ( info.GetDamageType() & DMG_DISSOLVE )
	{
		if ( m_hRagdoll )
			m_hRagdoll->GetBaseAnimating()->Dissolve( NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
	}

	CBaseEntity *pAttacker = info.GetAttacker();

	if ( pAttacker )
	{
		int iScoreToAdd = 1;
		if ( pAttacker == this )
			iScoreToAdd = -1;

		GetGlobalTeam( pAttacker->GetTeamNumber() )->AddScore( iScoreToAdd );
	}

	FlashlightTurnOff();

	m_lifeState = LIFE_DEAD;

	RemoveEffects( EF_NODRAW );	// still draw player body
}

int CTFCSPlayer::OnTakeDamage( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo inputInfoCopy( inputInfo );

	bool bTookDamage = ( BaseClass::OnTakeDamage( inputInfoCopy ) != 0 );

	// Early out if the base class took no damage
	if ( !bTookDamage )
		return 0;

	m_vecTotalBulletForce += inputInfo.GetDamageForce();
	
	gamestats->Event_PlayerDamage( this, inputInfo );

	return bTookDamage;
}

int CTFCSPlayer::OnTakeDamage_Alive( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo info = inputInfo;
	float flDamage = info.GetDamage();

	// Early out if no damage
	if ( flDamage == 0 )
		return 0;

	// Self damage (rocket/grenade jumps) do less damage to yourself
	if ( info.GetAttacker() == this && (info.GetDamageType() & DMG_BLAST))
		flDamage *= TFCS_SELF_DAMAGE_MULTIPLIER;

	// Demomen are more resistant to explosions
	if ( m_Shared.GetClassIndex() == CLASS_DEMOMAN && (info.GetDamageType() & DMG_BLAST))
		flDamage *= TFCS_DEMOMAN_EXPLOSION_MULTIPLIER;

	// Pyros take less direct fire damage
	if ( m_Shared.GetClassIndex() == CLASS_PYRO && (info.GetDamageType() & DMG_BURN) )
		flDamage *= TFCS_PYRO_FIRE_RESIST_MULTIPLIER;

	// Deal with Armour
	if ( !(info.GetDamageType() & (DMG_FALL | DMG_DROWN | DMG_POISON | DMG_RADIATION)) && flDamage > 0)	// armor doesn't protect against fall or drown damage! also don't increase armor incase of negative damage
	{
		float flArmorDamage = flDamage * m_ArmorClass;
		float flCurrentArmor = ArmorValue();
		flDamage = flDamage * (1 - m_ArmorClass) - min( 0, flCurrentArmor - flArmorDamage );

		SetArmorValue( max( 0, flCurrentArmor - flArmorDamage ) );

		info.SetDamage( flDamage );
	}

	// TODO: Deal with protection powerup; Player takes only armor damage
	// TODO: Deal with biosuit powerup; Player does not take drown or nervegas damage
	// TODO: Figure out proper jumping for explosions using the original damage and not the amount after all reductions
	
	return BaseClass::OnTakeDamage_Alive( info );
}

bool CTFCSPlayer::ShouldGib( const CTakeDamageInfo &info )
{
	if ( info.GetDamageType() & DMG_NEVERGIB )
		return false;

	if ( info.GetDamageType() & DMG_ALWAYSGIB )
		return true;

	if ( g_pGameRules->Damage_ShouldGibCorpse( info.GetDamageType() ) )
	{
		if ( m_iHealth < -2 )
			return true;
	}

	return false;
}

void CTFCSPlayer::DeathSound( const CTakeDamageInfo &info )
{
	if ( m_hRagdoll && m_hRagdoll->GetBaseAnimating()->IsDissolving() )
		 return;

	BaseClass::DeathSound( info );
}

bool CTFCSPlayer::SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot )
{
	// Get an initial spawn point.
	pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	if ( !pSpot )
	{
		// Sometimes the first spot can be NULL????
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
	}

	// First we try to find a spawn point that is fully clear. If that fails,
	// we look for a spawnpoint that's clear except for another players. We
	// don't collide with our team members, so we should be fine.
	bool bIgnorePlayers = false;

	CBaseEntity *pFirstSpot = pSpot;
	do 
	{
		if ( pSpot )
		{
			// Check to see if this is a valid team spawn ( player is on this team, etc. ).
			if( TFCSGameRules()->IsSpawnPointValid( pSpot, this, bIgnorePlayers ) )
			{
				// Check for a bad spawn entity.
				if ( pSpot->GetAbsOrigin() == vec3_origin )
				{
					pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
					continue;
				}

				if ( bIgnorePlayers )
				{
					// We're spawning on a busy spawn point so kill off anyone occupying it.
					edict_t	*edPlayer;
					edPlayer = edict();
					CBaseEntity *ent = NULL;
					for ( CEntitySphereQuery sphere( pSpot->GetAbsOrigin(), 128 ); ( ent = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
					{
						// if ent is a client, kill 'em (unless they are ourselves)
						if ( ent->IsPlayer() && !( ent->edict() == edPlayer ) )
						{
							CTakeDamageInfo info( this, this, 1000, DMG_CRUSH );
							ent->TakeDamage( info );
						}
					}
				}

				// Found a valid spawn point.
				return true;
			}
		}

		// Get the next spawning point to check.
		pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );

		if ( pSpot == pFirstSpot && !bIgnorePlayers )
		{
			// Loop through again, ignoring players
			bIgnorePlayers = true;
			pSpot = gEntList.FindEntityByClassname( pSpot, pEntClassName );
		}
	} 
	// Continue until a valid spawn point is found or we hit the start.
	while ( pSpot != pFirstSpot ); 

	return false;
}

CBaseEntity* CTFCSPlayer::EntSelectSpawnPoint( void )
{
	CBaseEntity *pSpot = g_pLastSpawnPoints[ GetTeamNumber() ];
	const char *pSpawnPointName = "";

	switch( GetTeamNumber() )
	{
	case TEAM_RED:
	case TEAM_BLUE:
	case TEAM_GREEN:
	case TEAM_YELLOW:
		{
			bool bSuccess = false;

			pSpawnPointName = "info_player_teamspawn";
			bSuccess = SelectSpawnSpot( pSpawnPointName, pSpot );

			if ( bSuccess )
				g_pLastSpawnPoints[ GetTeamNumber() ] = pSpot;

			break;
		}
	case TEAM_SPECTATOR:
	case TEAM_UNASSIGNED:
	default:
		{
			pSpot = CBaseEntity::Instance( INDEXENT(0) );
			break;		
		}
	}

	if ( !pSpot )
	{
		Warning( "PutClientInServer: no %s on level\n", pSpawnPointName );
		return CBaseEntity::Instance( INDEXENT( 0 ) );
	}

	return pSpot;
} 

//-----------------------------------------------------------------------------
// Purpose: Override setup bones so that is uses the render angles from
//			the TFCS animation state to setup the hitboxes.
//-----------------------------------------------------------------------------
void CTFCSPlayer::SetupBones( matrix3x4_t *pBoneToWorld, int boneMask )
{
	VPROF_BUDGET( "CTFCSPlayer::SetupBones", VPROF_BUDGETGROUP_SERVER_ANIM );

	// Set the mdl cache semaphore.
	MDLCACHE_CRITICAL_SECTION();

	// Get the studio header.
	Assert( GetModelPtr() );
	CStudioHdr *pStudioHdr = GetModelPtr( );
	if ( !pStudioHdr )
		return;

	Vector pos[MAXSTUDIOBONES];
	Quaternion q[MAXSTUDIOBONES];

	// Adjust hit boxes based on IK driven offset.
	Vector adjOrigin = GetAbsOrigin() + Vector( 0, 0, m_flEstIkOffset );

	// FIXME: pass this into Studio_BuildMatrices to skip transforms
	CBoneBitList boneComputed;
	if ( m_pIk )
	{
		m_iIKCounter++;
		m_pIk->Init( pStudioHdr, GetAbsAngles(), adjOrigin, gpGlobals->curtime, m_iIKCounter, boneMask );
		GetSkeleton( pStudioHdr, pos, q, boneMask );

		m_pIk->UpdateTargets( pos, q, pBoneToWorld, boneComputed );
		CalculateIKLocks( gpGlobals->curtime );
		m_pIk->SolveDependencies( pos, q, pBoneToWorld, boneComputed );
	}
	else
	{
		GetSkeleton( pStudioHdr, pos, q, boneMask );
	}

	CBaseAnimating *pParent = dynamic_cast< CBaseAnimating* >( GetMoveParent() );
	if ( pParent )
	{
		// We're doing bone merging, so do special stuff here.
		CBoneCache *pParentCache = pParent->GetBoneCache();
		if ( pParentCache )
		{
			BuildMatricesWithBoneMerge( 
				pStudioHdr, 
				GetAnimState()->GetRenderAngles(),
				adjOrigin, 
				pos, 
				q, 
				pBoneToWorld, 
				pParent, 
				pParentCache );

			return;
		}
	}

	Studio_BuildMatrices( 
		pStudioHdr, 
		GetAnimState()->GetRenderAngles(),
		adjOrigin, 
		pos, 
		q, 
		-1,
		GetModelScale(), // Scaling
		pBoneToWorld,
		boneMask );
}

int CTFCSPlayer::TakeHealth( float flHealth )
{
	return BaseClass::TakeHealth( flHealth, DMG_GENERIC );
}

void CTFCSPlayer::Heal( void )
{
	//TODO: Make this take a healer handle for assists later on
	int iCurrentHealth = GetHealth();
	int iMaxHealth = GetMaxHealth();

	//Heal very quickly if below max health else overheal slightly
	if ( iCurrentHealth < iMaxHealth )
	{
		SetHealth( min( iMaxHealth, iCurrentHealth + TFCS_MEDIKIT_HEAL ) );
	}
	else
	{
		SetHealth( min( iCurrentHealth + TFCS_MEDIKIT_OVERHEAL, iMaxHealth + TFCS_MEDIKIT_MAX_OVERHEAL ) );
	}
}

int CTFCSPlayer::TakeArmor( int iArmor )
{
	int iOldArmor = ArmorValue();
	int iMax = GetClassData( m_Shared.GetClassIndex() )->m_iMaxArmor;
	
	IncrementArmorValue( iArmor, iMax );

	return min(iArmor, ArmorValue() - iOldArmor);
}

int CTFCSPlayer::GiveAmmo( int iCount, int iAmmoIndex, bool bSupressSound )
{
	//Don't take more than our class data says
	int iMax = GetClassData(m_Shared.GetClassIndex())->m_aMaxAmmo[iAmmoIndex];
	int iCurrent = GetAmmoCount( iAmmoIndex );

	return BaseClass::GiveAmmo( min( iCount, iMax - iCurrent ), iAmmoIndex, bSupressSound );
}

//Sets the player's armor class to either the picked up armor class or the class's max armor class
void CTFCSPlayer::SetArmorClass( float flArmorClass )
{
	float flMaxArmorClass = GetClassData( m_Shared.GetClassIndex() )->m_flArmorClass;
	m_ArmorClass = min( flArmorClass, flMaxArmorClass );
	//m_ArmorClass = flArmorClass;
}

int CTFCSPlayer::GetMaxArmor( void )
{
	return GetClassData( m_Shared.GetClassIndex() )->m_iMaxArmor;
}

void CTFCSPlayer::PhysObjectSleep()
{
	IPhysicsObject *pObj = VPhysicsGetObject();
	if ( pObj )
		pObj->Sleep();
}

void CTFCSPlayer::PhysObjectWake()
{
	IPhysicsObject *pObj = VPhysicsGetObject();
	if ( pObj )
		pObj->Wake();
}

void CTFCSPlayer::State_Transition( TFCSPlayerState newState )
{
	State_Leave();
	State_Enter( newState );
}

void CTFCSPlayer::State_Enter( TFCSPlayerState newState )
{
	m_iPlayerState = newState;
	m_pCurStateInfo = State_LookupInfo( newState );

	if ( tfcs_showstatetransitions.GetInt() == -1 || tfcs_showstatetransitions.GetInt() == entindex() )
	{
		if ( m_pCurStateInfo )
			Msg( "ShowStateTransitions: entering '%s'\n", m_pCurStateInfo->m_pStateName );
		else
			Msg( "ShowStateTransitions: entering #%d\n", newState );
	}

	// Initialize the new state.
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnEnterState )
		(this->*m_pCurStateInfo->pfnEnterState)();
}

void CTFCSPlayer::State_Leave( void )
{
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnLeaveState )
	{
		(this->*m_pCurStateInfo->pfnLeaveState)();
	}
}

CTFCSPlayerStateInfo *CTFCSPlayer::State_LookupInfo( TFCSPlayerState state )
{
	// This table MUST match the 
	static CTFCSPlayerStateInfo playerStateInfos[] =
	{
		{ STATE_ACTIVE, "STATE_ACTIVE", &CTFCSPlayer::State_Enter_ACTIVE, NULL, &CTFCSPlayer::State_PreThink_ACTIVE },
		{ STATE_WELCOME, "STATE_WELCOME", &CTFCSPlayer::State_Enter_WELCOME, NULL, &CTFCSPlayer::State_PreThink_WELCOME },
		{ STATE_PICKINGTEAM, "STATE_PICKINGTEAM", &CTFCSPlayer::State_Enter_PICKINGTEAM, NULL, &CTFCSPlayer::State_PreThink_WELCOME },
		{ STATE_PICKINGCLASS, "STATE_PICKINGCLASS", &CTFCSPlayer::State_Enter_PICKINGCLASS, NULL, &CTFCSPlayer::State_PreThink_WELCOME },
		{ STATE_DEATH_ANIM, "STATE_DEATH_ANIM", &CTFCSPlayer::State_Enter_DEATH_ANIM, NULL, &CTFCSPlayer::State_PreThink_DEATH_ANIM },
		{ STATE_OBSERVER_MODE, "STATE_OBSERVER_MODE", &CTFCSPlayer::State_Enter_OBSERVER_MODE, NULL, &CTFCSPlayer::State_PreThink_OBSERVER_MODE }
	};

	for ( int i = 0; i < ARRAYSIZE( playerStateInfos ); i++ )
	{
		if ( playerStateInfos[i].m_iPlayerState == state )
			return &playerStateInfos[i];
	}

	return NULL;
}

void CTFCSPlayer::State_Enter_WELCOME()
{
	// Important to set MOVETYPE_NONE or our physics object will fall while we're sitting at one of the intro cameras.
	SetMoveType( MOVETYPE_NONE );
	AddSolidFlags( FSOLID_NOT_SOLID );

	PhysObjectSleep();

	// Show info panel
	if ( IsBot() )
	{
		// If they want to auto join a team for debugging, pretend they clicked the button.
		CCommand args;
		args.Tokenize( "joingame" );
		ClientCommand( args );
	}
	else
	{
		const ConVar *hostname = cvar->FindVar( "hostname" );
		const char *title = (hostname) ? hostname->GetString() : "MESSAGE OF THE DAY";

		// open info panel on client showing MOTD:
		KeyValues *data = new KeyValues( "data" );
		data->SetString( "title", title );		// info panel title
		data->SetString( "type", "1" );			// show userdata from stringtable entry
		data->SetString( "msg", "motd" );		// use this stringtable entry
		//data->SetString( "cmd", TEXTWINDOW_CMD_JOINGAME );// exec this command if panel closed
		CCommand args;
		args.Tokenize( "joingame" );
		ClientCommand( args );

		ShowViewPortPanel( PANEL_INFO, true, data );

		data->deleteThis();

	}
}

void CTFCSPlayer::State_PreThink_WELCOME()
{
	//TODO: Camera stuff
}

void CTFCSPlayer::State_Enter_PICKINGTEAM()
{
	ShowViewPortPanel( PANEL_TEAM );
	PhysObjectSleep();
}

void CTFCSPlayer::State_Enter_PICKINGCLASS()
{
	ShowClassSelectMenu();
	PhysObjectSleep();
}

void CTFCSPlayer::State_Enter_ACTIVE()
{

}
void CTFCSPlayer::State_PreThink_ACTIVE()
{

}

void CTFCSPlayer::State_Enter_OBSERVER_MODE()
{

}
void CTFCSPlayer::State_PreThink_OBSERVER_MODE()
{

}

void CTFCSPlayer::State_Enter_DEATH_ANIM()
{

}
void CTFCSPlayer::State_PreThink_DEATH_ANIM()
{

}