#include "cbase.h"
#include "tfcs_shareddefs.h"

const char* g_aClassNames[11] =
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
	"Civillian"
};

const char* g_aTeamNames[] =
{
	"#SDK_Team_Unassigned",
	"#SDK_Team_Spectator",
	"#SDK_Team_Blue",
	"#SDK_Team_Red",
	"#SDK_Team_Green",
	"#SDK_Team_Yellow"
};

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