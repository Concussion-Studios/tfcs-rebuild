#include "cbase.h"
#include "tfcs_playerclass_parse.h"
#include "tfcs_weapon_base.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

TFCSPlayerClassInfo_t g_aTFCSPlayerClassData[CLASS_LAST];

TFCSPlayerClassInfo_t::TFCSPlayerClassInfo_t()
{
	m_flMaxSpeed = 0.0f;
	m_iMaxArmor = 0;
	m_iMaxHealth = 0;
	m_iSpawnArmor = 0;
	m_flArmorClass = 0.0f;
	m_szArmsModel[0] = '\0';
	m_szClassName[0] = '\0';
	// Grenades
	m_iGrenType1 = WEAPON_NONE;
	m_iGrenType2 = WEAPON_NONE;

	for (int i = 0; i < TFCS_MAX_WEAPON_SLOTS; i++)
	{
		m_aWeapons[i] = WEAPON_NONE;
	}

	for (int i = 0; i < AMMO_LAST; i++)
	{
		m_aMaxAmmo[i] = 0;
		m_aSpawnAmmo[i] = 0;
	}
}

void TFCSPlayerClassInfo_t::Parse(const char *szName)
{
	//No need to parse twice
	if (m_bParsed)
		return;

	const unsigned char *pKey = NULL;

	if (g_pGameRules)
	{
		pKey = g_pGameRules->GetEncryptionKey();
	}

	KeyValues *pKeyValues = ReadEncryptedKVFile(filesystem, szName, pKey);
	if (pKeyValues)
	{
		ParseData(pKeyValues);
		pKeyValues->deleteThis();
	}
}

void TFCSPlayerClassInfo_t::ParseData(KeyValues *pKeyValuesData)
{	
	Q_strncpy(m_szArmsModel, pKeyValuesData->GetString("armsmodel", "!! Missing armsmodel on Player Class"), MAX_PLAYERCLASS_NAME_LENGTH);
	Assert(Q_strlen(m_szArmsModel) > 0 && "Every class must specify a armsmodel");
	
	m_flMaxSpeed = pKeyValuesData->GetFloat("speed_max", 220.0f);
	m_iMaxHealth = pKeyValuesData->GetInt("health_max", 100);
	m_iMaxArmor = pKeyValuesData->GetInt("armor_max", 100);
	m_iSpawnArmor = pKeyValuesData->GetInt("armor_spawn", 0);
	m_flArmorClass = pKeyValuesData->GetFloat("armor_class", 0.3f);
	
	KeyValues *pKeyValuesAmmo = pKeyValuesData->FindKey("MaxAmmo");
	KeyValues *pKeyValuesSpawnAmmo = pKeyValuesData->FindKey("SpawnAmmo");
	
	//Get max ammo carried by class
	if (pKeyValuesAmmo)
	{
		for (int i = 1; i < AMMO_LAST; ++i)
		{
			m_aMaxAmmo[i] = pKeyValuesAmmo->GetInt(g_aAmmoNames[i], 0);
		}
	}
	
	//Get ammo that player will spawn with
	if (pKeyValuesSpawnAmmo)
	{
		for (int i = 1; i < AMMO_LAST; ++i)
		{
			m_aSpawnAmmo[i] = pKeyValuesSpawnAmmo->GetInt(g_aAmmoNames[i], 0);
		}
	}
	
	char buf[32];
	for (int i = 0; i < TFCS_MAX_WEAPON_SLOTS; i++)
	{
		Q_snprintf(buf, sizeof(buf), "weapon%d", i + 1);
	}
	
	const char *pszGrenType1 = pKeyValuesData->GetString("grenade1", "WEAPON_NONE");
	m_iGrenType1 = AliasToWeaponID(pszGrenType1);
	
	const char *pszGrenType2 = pKeyValuesData->GetString("grenade2", "WEAPON_NONE");
	m_iGrenType2 = AliasToWeaponID(pszGrenType2);

	m_bParsed = true;
}
//////////////////////////////////////////////////////////////////
//Player Class shared
//////////////////////////////////////////////////////////////////

#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE(CTFCSPlayerClass, DT_TFCSPlayerClass)
RecvPropInt(RECVINFO(m_iClass)),
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE(CTFCSPlayerClass, DT_TFCSPlayerClass)
SendPropInt(SENDINFO(m_iClass), Q_log2(CLASS_LAST)+1, SPROP_UNSIGNED),
END_SEND_TABLE()
#endif

CTFCSPlayerClass::CTFCSPlayerClass()
{
	m_iClass.Set(CLASS_FIRST);
}

TFCSPlayerClassInfo_t *CTFCSPlayerClass::GetData(int iClass)
{
	Assert(iClass > 0 && iClass <= CLASS_LAST);
	return &g_aTFCSPlayerClassData[iClass];
}

bool CTFCSPlayerClass::Init(int iClass)
{
	Assert((iClass >= CLASS_FIRST) && iClass < CLASS_LAST);
	m_iClass = iClass;
	return true;
}