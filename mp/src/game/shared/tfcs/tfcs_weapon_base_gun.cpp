#include "cbase.h"
#include "tfcs_weapon_base_gun.h"

//Data tables
IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponBaseGun, DT_TFCSWeaponBaseGun )
BEGIN_NETWORK_TABLE( CTFCSWeaponBaseGun, DT_TFCSWeaponBaseGun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponBaseGun )
END_PREDICTION_DATA()

CTFCSWeaponBaseGun::CTFCSWeaponBaseGun()
{

}

CTFCSWeaponBaseGun::~CTFCSWeaponBaseGun()
{

}

void CTFCSWeaponBaseGun::PrimaryAttack( void )
{
	// Check if player can fire

	// Call FireWeapon

	// Set next fire time
}

void CTFCSWeaponBaseGun::SecondaryAttack( void )
{

}

bool CTFCSWeaponBaseGun::StartReload( void )
{
	// Check if weapon has a full clip

	// Check if player has enough ammo

	// Set begin reload time
	return true;
}

bool CTFCSWeaponBaseGun::Reload( void )
{
	// Remove ammo from player

	// Add ammo to clip

	// Set next reload time
	return true;
}

void CTFCSWeaponBaseGun::FinishReload( void )
{
	// Set next fire time
}