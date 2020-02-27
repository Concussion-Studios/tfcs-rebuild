#include "cbase.h"
#include <KeyValues.h>
#include "tfcs_weapon_parse.h"
#include "tfcs_shareddefs.h"

CTFCSWeaponInfo::CTFCSWeaponInfo()
{
}

CTFCSWeaponInfo::~CTFCSWeaponInfo()
{
}

void CTFCSWeaponInfo::Parse(KeyValues *pKeyValuesData, const char *szWeaponName)
{
	BaseClass::Parse(pKeyValuesData, szWeaponName);
	m_nDamage = pKeyValuesData->GetInt("Damage", 0);
	m_nBulletsPerShot = pKeyValuesData->GetInt("BulletsPerShot", 1);
	m_flRange = pKeyValuesData->GetFloat("Range", 8192.0f);
	m_nAmmoPerShot = pKeyValuesData->GetInt("AmmoPerShot", 1);
	m_nAmmoPerReload = pKeyValuesData->GetInt("AmmoPerReload", 1);
	m_flReloadSpeed = pKeyValuesData->GetFloat("ReloadSpeed", 0.5f);
	m_flDrawTime = pKeyValuesData->GetFloat("DrawTime", 0.1f);
}