#ifndef TFCS_PLAYERCLASS_PARSE_H
#define TFCS_PLAYERCLASS_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_shareddefs.h"
#include "playerclass_info_parse.h"
#include "networkvar.h"

struct TFCSPlayerClassInfo_t
{
	char m_szArmsModel[MAX_PLAYERCLASS_NAME_LENGTH];
	char m_szLimitCvar[MAX_PLAYERCLASS_NAME_LENGTH];	//which cvar controls the class limit for this class

	float m_flMaxSpeed;
	int m_iMaxArmor;
	int m_iMaxHealth;
	int m_iSpawnArmor;
	float m_flArmorClass;
	int m_aMaxAmmo[AMMO_LAST];
	int m_aSpawnAmmo[AMMO_LAST];

	//Weapons
	int m_aWeapons[TFCS_MAX_WEAPON_SLOTS];

	// Grenades
	int m_iGrenType1;
	int m_iGrenType2;

	TFCSPlayerClassInfo_t();
	void Parse(KeyValues *pKeyValues, const char *pszClassName);
};

class CTFCSPlayerClass
{
public:
	CTFCSPlayerClass();
	DECLARE_EMBEDDED_NETWORKVAR()
	DECLARE_CLASS_NOBASE(CTFCSPlayerClass);

	int GetClassIndex(void) { return m_iClass; }
	
	TFCSPlayerClassInfo_t *GetData(int iClass);

	CNetworkVar(int, m_iClass);
};
#endif //TFCS_PLAYERCLASS_PARSE_H