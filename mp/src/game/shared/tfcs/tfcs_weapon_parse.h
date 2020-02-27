#ifndef TFCS_WEAPON_PARSE_H
#define TFCS_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"
#include "networkvar.h"

class CTFCSWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT(CTFCSWeaponInfo, FileWeaponInfo_t);

	CTFCSWeaponInfo();
	~CTFCSWeaponInfo();

	virtual void Parse(::KeyValues *pKeyValuesData, const char *szWeaponName);

	int m_nDamage;
	int m_nBulletsPerShot;
	float m_flRange;
	float m_flSpread;
	int m_nAmmoPerShot;
	int m_nAmmoPerReload;
	float m_flReloadSpeed;
	float m_flDrawTime;
};
#endif //TFCS_WEAPON_PARSE_H