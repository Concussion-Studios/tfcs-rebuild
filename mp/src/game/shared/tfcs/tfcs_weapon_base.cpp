//***************************************************************
//
//	TFCS Weapon Base Code
//
//	CTFCSWeaponBase
//	|
//	+-CTFCSWeaponBaseMelee
//	| |
//  | +-CTFCSWeaponCrowbar
//	| |
//  | +-CTFCSWeaponMedikit
//	| |
//  | +-CTFCSWeaponSpanner
//	| |
//  | +-CTFCSWeaponUmbrella
//	| |
//  | +-CTFCSWeaponKnife
//	|
//  +-CTFCSWeaponBaseGun
//	| |
//  | +-CTFCSWeaponShotgun
//	| |
//  | +-CTFCSWeaponRailgun
//	| |
//  | +-CTFCSWeaponAutorifle
//	| |
//  | +-CTFCSWeaponTranq
//	| |
//  | +-CTFCSWeaponSuperShotgun
//	| |
//  | +-CTFCSWeaponSniperrifle
//	| |
//  | +-CTFCSWeaponFlamethrower
//	| |
//  | +-CTFCSWeaponNailgun
//	| |
//  | +-CTFCSWeaponSuperNailgun
//	| |
//  | +-CTFCSWeaponGrenadeLauncher
//	| |
//  | +-CTFCSWeaponRocketLauncher
//	| |
//  | +-CTFCSWeaponPipebombLauncher
//	| |
//  | +-CTFCSWeaponAssaultCannon
//	| |
//  | +-CTFCSWeaponIncindiaryCannon
//	|
//  +-CTFCSWeaponBaseGrenade
//	  |
//    Rest of the throwable nades
//
//***************************************************************

#include "cbase.h"
#include "tfcs_weapon_base.h"

//Data tables
IMPLEMENT_NETWORKCLASS_ALIASED(TFCSWeaponBase, DT_TFCSWeaponBase)
BEGIN_NETWORK_TABLE(CTFCSWeaponBase, DT_TFCSWeaponBase)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFCSWeaponBase)
END_PREDICTION_DATA()

#ifdef GAME_DLL
BEGIN_DATADESC(CTFCSWeaponBase)
//DEFINE_ENTITYFUNC(DropThink),
END_DATADESC()
#endif

CTFCSWeaponBase::CTFCSWeaponBase()
{
	SetPredictionEligible(true);
	AddSolidFlags(FSOLID_TRIGGER);

	m_bFiresUnderwater = true;
	//m_bMuzzleFlash = true;

	SetCollisionGroup(COLLISION_GROUP_WEAPON);
}

void CTFCSWeaponBase::Spawn(void)
{
	BaseClass::Spawn();
}

const CTFCSWeaponInfo &CTFCSWeaponBase::GetTFCSWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CTFCSWeaponInfo *pInfo = static_cast<const CTFCSWeaponInfo *>(pWeaponInfo);

	return *pInfo;
}

void CTFCSWeaponBase::PrimaryAttack(void)
{
	BaseClass::PrimaryAttack();
}

#ifdef CLIENT_DLL
bool CTFCSWeaponBase::ShouldPredict()
{
	if(GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer())
	{
		return true;
	}

	return BaseClass::ShouldPredict();
}

void CTFCSWeaponBase::OnDataChanged(DataUpdateType_t type)
{
	BaseClass::OnDataChanged(type);

	if (GetPredictable() && !ShouldPredict())
	{
		ShutdownPredictable();
	}
}
#endif