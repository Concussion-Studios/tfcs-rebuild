#include "cbase.h"
#include "tfcs_player.h"
#include "tfcs_player_shared.h"
#include "tfcs_gamerules.h"
#include "tfcs_playerclass_parse.h"
#include "tfcs_shareddefs.h"
//#include "multiplayer_animstate.h"
#include "keyvalues.h"
#include "viewport_panel_names.h"
#include "client.h"
#include "team.h"
#include "GameStats.h"
#include "datacache/imdlcache.h"

#define TFCS_PLAYER_MODEL "models/player/scout.mdl"

#define TFCS_SELF_DAMAGE_MULTIPLIER 0.75
#define TFCS_DEMOMAN_EXPLOSION_MULTIPLIER 0.85
#define TFCS_PYRO_FIRE_RESIST_MULTIPLIER 0.5

#define TFCS_MEDIKIT_HEAL 200
#define TFCS_MEDIKIT_OVERHEAL 10
#define TFCS_MEDIKIT_MAX_OVERHEAL 50

#pragma warning(disable:4189) // TODO: fix this error

EHANDLE g_pLastSpawnPoints[TEAM_COUNT];

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

	CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name ) {}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nData ), 32 )
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( pPlayer->EyePosition() );

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.m_nData = nData;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Filters updates to a variable so that only non-local players see
// the changes.  This is so we can send a low-res origin to non-local players
// while sending a hi-res one to the local player.
// Input  : *pVarData -
//			*pOut -
//			objectID -
//-----------------------------------------------------------------------------

void* SendProxy_SendNonLocalDataTable( const SendProp *pProp, const void *pStruct, const void *pVarData, CSendProxyRecipients *pRecipients, int objectID )
{
	pRecipients->SetAllRecipients();
	pRecipients->ClearRecipient( objectID - 1 );
	return (void *)pVarData;
}
REGISTER_SEND_PROXY_NON_MODIFIED_POINTER( SendProxy_SendNonLocalDataTable );

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //
BEGIN_DATADESC( CTFCSPlayer )
	DEFINE_FIELD( m_ArmorClass, FIELD_FLOAT ),
	//DEFINE_FIELD( m_ArmorValue, FIELD_INTEGER ),
	DEFINE_FIELD( m_MaxArmor, FIELD_INTEGER ),
	DEFINE_FIELD( m_flConcussTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_flCrippleTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_iCrippleLevel, FIELD_INTEGER ),
END_DATADESC()

BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSLocalPlayerExclusive )
	//SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropFloat( SENDINFO( m_ArmorClass ) ),
	//SendPropInt( SENDINFO( m_Armor ), 8, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_MaxArmor ), 8, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flConcussTime ) ),
END_SEND_TABLE()

//BEGIN_SEND_TABLE_NOBASE( CTFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
//END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CTFCSPlayer, DT_TFCSPlayer )

	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),

	SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),

	SendPropAngle( SENDINFO_VECTORELEM( m_angEyeAngles, 0 ), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM( m_angEyeAngles, 1 ), 11 ),

	SendPropInt( SENDINFO( m_iRealSequence ), 9 ),

	// Data that only gets sent to the local player.
	SendPropDataTable( SENDINFO_DT( m_Shared ), &REFERENCE_SEND_TABLE( DT_TFCSPlayerShared ) ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "tfcs_localdata", 0, &REFERENCE_SEND_TABLE( DT_TFCSLocalPlayerExclusive ), SendProxy_SendLocalDataTable ),
	//SendPropDataTable( "tfcs_nonlocaldata", 0, &REFERENCE_SEND_TABLE( DT_TFCSNonLocalPlayerExclusive ), SendProxy_SendNonLocalDataTable ),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( player, CTFCSPlayer );
PRECACHE_REGISTER( player );

ITFCSPlayerAnimState* CreatePlayerAnimState( CTFCSPlayer *pPlayer );

CTFCSPlayer::CTFCSPlayer()
{
	m_PlayerAnimState = CreatePlayerAnimState( this );
	UseClientSideAnimation();
	m_angEyeAngles.Init();

	m_lifeState = LIFE_DEAD; // Start "dead".

	m_Shared.Init( this );

	//SetViewOffset( TFC_PLAYER_VIEW_OFFSET );

	SetContextThink( &CTFCSPlayer::TFCSPlayerThink, gpGlobals->curtime, "TFCSPlayerThink" );
}

CTFCSPlayer::~CTFCSPlayer()
{
	m_PlayerAnimState->Release();
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

void CTFCSPlayer::Precache()
{
	// Precache player models
	for ( int i = 0; i < CLASS_LAST; i++ )
	{
		const char *pszModel = GetClassData( i )->m_szModelName;
		if ( pszModel && pszModel[0] )
		{
			PrecacheModel( pszModel );
		}
	}

	BaseClass::Precache();
}

void CTFCSPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn();
}

void CTFCSPlayer::Spawn()
{
	MDLCACHE_CRITICAL_SECTION();
	SetModel( GetClassData( m_Shared.GetClassIndex() )->m_szModelName );

	SetMoveType( MOVETYPE_WALK );
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	BaseClass::Spawn();

	switch ( GetTeamNumber() )
	{
	case TEAM_RED:
		m_nSkin = 0;
		break;
	case TEAM_BLUE:
		m_nSkin = 1;
		break;
	case TEAM_GREEN:
		m_nSkin = 2;
		break;
	case TEAM_YELLOW:
		m_nSkin = 3;
		break;
	}

	InitClass();

	Vector mins = VEC_HULL_MIN;
	Vector maxs = VEC_HULL_MAX;

	CollisionProp()->SetSurroundingBoundsType( USE_SPECIFIED_BOUNDS, &mins, &maxs );
}

// Set the player up with the default weapons, ammo, etc.
void CTFCSPlayer::GiveDefaultItems()
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
	TFCSPlayerClassInfo_t *data = GetClassData( CLASS_SCOUT );
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
		{
			ClientPrint( this, HUD_PRINTTALK, "#game_respawn_asrandom" );
		}
		else
		{
			ClientPrint( this, HUD_PRINTTALK, "#game_spawn_asrandom" );
		}
	}
	else
	{
		if ( IsAlive() )
		{
			ClientPrint( this, HUD_PRINTTALK, "#game_respawn_as", classname );
		}
		else
		{
			ClientPrint( this, HUD_PRINTTALK, "#game_spawn_as", classname );
		}
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

void CTFCSPlayer::ForceRespawn()
{
	BaseClass::ForceRespawn();
}

void CTFCSPlayer::PreThink()
{
	BaseClass::PreThink();
}

void CTFCSPlayer::Think()
{
	BaseClass::Think();
}

void CTFCSPlayer::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
}

int CTFCSPlayer::OnTakeDamage( const CTakeDamageInfo &info )
{
	return BaseClass::OnTakeDamage( info );
}

int CTFCSPlayer::OnTakeDamage_Alive( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo info = inputInfo;
	float flDamage = info.GetDamage();

	// Early out if no damage
	if ( flDamage == 0 )
	{
		return 0;
	}

	// Self damage (rocket/grenade jumps) do less damage to yourself
	if ( info.GetAttacker() == this && (info.GetDamageType() & DMG_BLAST))
	{
		flDamage *= TFCS_SELF_DAMAGE_MULTIPLIER;
	}

	// Demomen are more resistant to explosions
	if ( m_Shared.GetClassIndex() == CLASS_DEMOMAN && (info.GetDamageType() & DMG_BLAST))
	{
		flDamage *= TFCS_DEMOMAN_EXPLOSION_MULTIPLIER;
	}

	// Pyros take less direct fire damage
	if ( m_Shared.GetClassIndex() == CLASS_PYRO && (info.GetDamageType() & DMG_BURN) )
	{
		flDamage *= TFCS_PYRO_FIRE_RESIST_MULTIPLIER;
	}

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

void CTFCSPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	//DoAnimationEvent( PLAYERANIMEVENT_DIE );

	BaseClass::Event_Killed( info );
}

void CTFCSPlayer::CommitSuicide( bool bExplode /* = false */, bool bForce /*= false*/ )
{
	//m_iSuicideCustomKillFlags = DMG_CUSTOM_SUICIDE;

	BaseClass::CommitSuicide( bExplode, bForce );
}

bool CTFCSPlayer::ClientCommand( const CCommand &args )
{
	const char *pcmd = args[0];

	if ( FStrEq( pcmd, "jointeam" ) )
	{

	}
	else if ( FStrEq( pcmd, "joinclass" ) )
	{
		if  (args.ArgC() < 2 )
		{
			Warning( "Player sent bad joinclass sntax\n" );
		}
		int iClass = atoi( args[1] );
		HandleCommand_JoinClass( iClass );
	}

	return BaseClass::ClientCommand( args );
}

void CTFCSPlayer::ChangeTeam( int iTeamNum )
{
	BaseClass::ChangeTeam( iTeamNum );
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

// Purpose: Find a spawn point for the player.
CBaseEntity* CTFCSPlayer::EntSelectSpawnPoint()
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