#ifndef TFCS_SHAREDDEFS_H
#define TFCS_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"
#include "mp_shareddefs.h"

//-----------------------------------------------------------------------------
// Gamerules.
//-----------------------------------------------------------------------------
#define TFCS_GAME_DESCRIPTION "Team Fortress Classic: Source"
#define TFCS_MAX_PLAYERS 128

//-----------------------------------------------------------------------------
// Player settings.
//-----------------------------------------------------------------------------
#define TFCS_DEFAULT_RUN_SPEED 220.0f
#define TFCS_MAX_WEAPON_SLOTS 10

#define TFCS_SELF_DAMAGE_MULTIPLIER 0.75
#define TFCS_DEMOMAN_EXPLOSION_MULTIPLIER 0.85
#define TFCS_PYRO_FIRE_RESIST_MULTIPLIER 0.5

#define TFCS_MEDIKIT_HEAL 200
#define TFCS_MEDIKIT_OVERHEAL 10
#define TFCS_MEDIKIT_MAX_OVERHEAL 50
#define TFCS_PLAYER_PHYSDAMAGE_SCALE 4.0f

// Player avoidance
#define PUSHAWAY_THINK_INTERVAL		( 1.0f / 20.0f )
#define TFCS_PUSHAWAY_THINK_INTERVAL		( 1.0f / 20.0f )

//-----------------------------------------------------------------------------
// Teams.
//-----------------------------------------------------------------------------
enum tfcsteams_e
{
	TEAM_BLUE = FIRST_GAME_TEAM,
	TEAM_RED,
	TEAM_GREEN,
	TEAM_YELLOW,

	TEAM_COUNT
};

#define TEAM_AUTOASSIGN ( TEAM_COUNT + 1 )

extern const char *g_aTeamNames[TEAM_COUNT];
extern color32 g_aTeamColors[TEAM_COUNT];

#define COLOR_SPECTATOR Color( 245, 229, 196, 255 )
#define COLOR_TFCS_BLUE	Color( 64, 64, 255, 255 )
#define COLOR_TFCS_RED	Color( 255, 64, 64, 255 )
#define COLOR_TFCS_GREEN	Color( 64, 255, 64, 255 )
#define COLOR_TFCS_YELLOW	Color( 255, 255, 64, 255 )

#define CONTENTS_REDTEAM	CONTENTS_TEAM1
#define CONTENTS_BLUETEAM	CONTENTS_TEAM2
#define CONTENTS_GREENTEAM	CONTENTS_UNUSED
#define CONTENTS_YELLOWTEAM	CONTENTS_UNUSED6

//-----------------------------------------------------------------------------
// TFCS-specific viewport panels
//-----------------------------------------------------------------------------
#define PANEL_CLASS_BLUE		"class_blue"
#define PANEL_CLASS_RED			"class_red"
#define PANEL_CLASS_GREEN		"class_green"
#define PANEL_CLASS_YELLOW		"class_yellow"

//-----------------------------------------------------------------------------
// The various states the player can be in during the join game process.
//-----------------------------------------------------------------------------
enum TFCSPlayerState
{
	STATE_ACTIVE = 0,
	STATE_WELCOME,				// Show the level intro screen.
	STATE_PICKINGTEAM,			// Choosing team.
	STATE_PICKINGCLASS,			// Choosing class.
	STATE_DEATH_ANIM,			// Playing death anim, waiting for that to finish.
	STATE_OBSERVER_MODE,		// Noclipping around, watching players, etc.
	NUM_PLAYER_STATES
};

//-----------------------------------------------------------------------------
// Player Classes.
//-----------------------------------------------------------------------------
#define CLASS_COUNT			( CLASS_LAST - 1 )
#define FIRST_NORMAL_CLASS	( CLASS_FIRST + 1 )
#define LAST_NORMAL_CLASS	( CLASS_CIVILLIAN - 1 )

enum
{
	CLASS_FIRST = 0,
	CLASS_SCOUT,			// FIRST_NORMAL_CLASS
	CLASS_SNIPER,
	CLASS_SOLDIER,
	CLASS_DEMOMAN,
	CLASS_MEDIC,
	CLASS_HWGUY,
	CLASS_PYRO,
	CLASS_SPY,
	CLASS_ENGINEER,			// LAST_NORMAL_CLASS
	CLASS_CIVILLIAN,

	CLASS_RANDOM,
	
	CLASS_LAST
};

extern const char *g_aClassNames[];					// localized class names
extern const char *g_aClassNames_NonLocalized[];	// non-localized class names

//-----------------------------------------------------------------------------
// Ammo types.
//-----------------------------------------------------------------------------
enum
{
	AMMO_DUMMY = 0,
	AMMO_SHELLS,
	AMMO_NAILS,
	AMMO_ROCKETS,
	AMMO_CELLS,
	AMMO_GRENADE1,
	AMMO_GRENADE2,
	AMMO_DETPACK,
	AMMO_LAST
};

extern const char* g_aAmmoNames[];

//-----------------------------------------------------------------------------
// Projectile types.
//-----------------------------------------------------------------------------
enum
{
	PROJECTILE_NONE = 0,
	PROJECTILE_BULLET,
	PROJECTILE_ROCKET,
	PROJECTILE_NAIL,
	PROJECTILE_GRENADE,
	PROJECTILE_PIPEBOMB,

	PROJECTILE_LAST
};

extern const char* g_aProjectileNames[];

//-----------------------------------------------------------------------------
// Weapon names.
//-----------------------------------------------------------------------------
int AliasToWeaponID( const char *alias );
const char *WeaponIDToAlias( int iWeaponID );

enum
{
	WEAPON_NONE = 0,
	WEAPON_CROWBAR,
	WEAPON_WRENCH,
	WEAPON_MEDIKIT,
	WEAPON_KNIFE,
	WEAPON_UMBRELLA,
	WEAPON_12GUAGE,
	WEAPON_RAILGUN,
	WEAPON_TRANQ,
	WEAPON_SNIPERRIFLE,
	WEAPON_AUTORIFLE,
	WEAPON_SHOTGUN,
	WEAPON_NAILGUN,
	WEAPON_GRENADELAUNCHER,
	WEAPON_FLAMETHROWER,
	WEAPON_SUPERNAILGUN,
	WEAPON_RPG,
	WEAPON_PIPEBOMBLAUNCHER,
	WEAPON_ASSAULTCANNON,
	WEAPON_INCINDIARYCANNON,
	WEAPON_GRENADE_NORMAL,
	WEAPON_GRENADE_CALTROP,
	WEAPON_GRENADE_NAIL,
	WEAPON_GRENADE_MIRV,
	WEAPON_GRENADE_CONCUSSION,
	WEAPON_GRENADE_NAPALM,
	WEAPON_GRENADE_HALLUCINATION,
	WEAPON_GREANDE_EMP,
	WEAPON_LAST
};

#endif //TFCS_SHAREDDEFS_H