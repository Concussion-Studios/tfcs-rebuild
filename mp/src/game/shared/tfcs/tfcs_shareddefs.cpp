#include "cbase.h"
#include "tfcs_shareddefs.h"

//-----------------------------------------------------------------------------
// Teams.
//-----------------------------------------------------------------------------
const char *g_aTeamNames[TEAM_COUNT] =
{
	"Unassigned",
	"Spectator",
	"Red",
	"Blue",
	"Green",
	"Yellow"
};

color32 g_aTeamColors[TEAM_COUNT] = 
{
	{ 0, 0, 0, 0 },		// Unassigned
	{ 0, 0, 0, 0 },		// Spectator
	{ 255, 0, 0, 0 },	// Red
	{ 0, 0, 255, 0 },	// Blue
	{ 0, 255, 0, 0 },	// Green
	{ 255, 255, 0, 0 }	// Yellow
};

//-----------------------------------------------------------------------------
// Classes.
//-----------------------------------------------------------------------------
const char *g_aClassNames[13] =
{
	"#TFCS_Class_Name_Undefined",
	"#TFCS_Class_Name_Scout",
	"#TFCS_Class_Name_Sniper",
	"#TFCS_Class_Name_Soldier",
	"#TFCS_Class_Name_Demoman",
	"#TFCS_Class_Name_Medic",
	"#TFCS_Class_Name_HWGuy",
	"#TFCS_Class_Name_Pyro",
	"#TFCS_Class_Name_Spy",
	"#TFCS_Class_Name_Engineer",
	"#TFCS_Class_Name_Civillian",
	"#TFCS_Class_Name_Random",
	NULL
};

const char* g_aClassNames_NonLocalized[13] =
{
	"None",
	"Sniper",
	"Scout",
	"Soldier",
	"Demoman",
	"Medic",
	"HWGuy",
	"Pyro",
	"Spy",
	"Engineer",
	"Civillian",
	"Random",
	NULL
};

//-----------------------------------------------------------------------------
// Ammo.
//-----------------------------------------------------------------------------
const char* g_aAmmoNames[] =
{
	"AMMO_NONE",
	"AMMO_SHELLS",
	"AMMO_NAILS",
	"AMMO_ROCKETS",
	"AMMO_NAILS",
	"AMMO_GRENADE1",
	"AMMO_GRENADE2",
	"AMMO_DETPACK",
	"AMMO_LAST"
};

//-----------------------------------------------------------------------------
// Projectiles.
//-----------------------------------------------------------------------------
const char* g_aProjectileNames[] =
{
	"PROJECTILE_NONE",
	"PROJECTILE_BULLET",
	"PROJECTILE_ROCKET",
	"PROJECTILE_NAIL",
	"PROJECTILE_GRENADE",
	"PROJECTILE_PIPEBOMB",
	"PROJECTILE_LAST"
};

//-----------------------------------------------------------------------------
// Weapons.
//-----------------------------------------------------------------------------
const char* g_aWeaponNames[] =
{
	"WEAPON_NONE",
	"WEAPON_CROWBAR",
	"WEAPON_WRENCH",
	"WEAPON_MEDIKIT",
	"WEAPON_KNIFE",
	"WEAPON_UMBRELLA",
	"WEAPON_12GUAGE",
	"WEAPON_RAILGUN",
	"WEAPON_TRANQ",
	"WEAPON_SNIPERRIFLE",
	"WEAPON_AUTORIFLE",
	"WEAPON_SHOTGUN",
	"WEAPON_NAILGUN",
	"WEAPON_GRENADELAUNCHER",
	"WEAPON_FLAMETHROWER",
	"WEAPON_SUPERNAILGUN",
	"WEAPON_RPG",
	"WEAPON_PIPEBOMBLAUNCHER",
	"WEAPON_ASSAULTCANNON",
	"WEAPON_INCINDIARYCANNON",
	"WEAPON_GRENADE_NORMAL",
	"WEAPON_GRENADE_CALTROP",
	"WEAPON_GRENADE_NAIL",
	"WEAPON_GRENADE_MIRV",
	"WEAPON_GRENADE_CONCUSSION",
	"WEAPON_GRENADE_NAPALM",
	"WEAPON_GRENADE_HALLUCINATION",
	"WEAPON_GREANDE_EMP",
	"WEAPON_LAST"
};

int AliasToWeaponID( const char *alias )
{
	if ( alias )
	{
		for ( int i = 0; i < WEAPON_LAST; ++i )
		{
			if ( !Q_stricmp( g_aWeaponNames[i], alias ) )
				return i;
		}
	}
	return WEAPON_NONE;
}

const char *WeaponIDToAlias( int iWeaponID )
{
	if ( iWeaponID <= 0 || iWeaponID > WEAPON_LAST )
		return NULL;

	return g_aWeaponNames[iWeaponID];
}