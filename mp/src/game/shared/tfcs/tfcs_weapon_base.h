#ifndef TFCS_WEAPON_BASE_H
#define TFCS_WEAPON_BASE_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_weapon_parse.h"

#ifdef CLIENT_DLL
#define CTFCSWeaponBase C_TFCSWeaponBase
#endif

class CTFCSWeaponBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CTFCSWeaponBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponBase();
	~CTFCSWeaponBase();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	virtual void Spawn( void );

	virtual bool IsPredicted() const { return true; }
	
	virtual int GetWeaponID() { Assert(0); return WEAPON_NONE; }

	CTFCSWeaponInfo const &GetTFCSWpnData() const;

	//TODO Get Owner

	virtual void PrimaryAttack();

	//virtual void Drop( const Vector &vecVelocity ); //todo: make weapons drop on owner death
	//void DropThink( void );

	virtual float GetFOV() { return -1; }

private:

	CTFCSWeaponBase( const CTFCSWeaponBase & );

#ifdef CLIENT_DLL
	virtual bool ShouldPredict();
	virtual void OnDataChanged( DataUpdateType_t type );
#endif
};
#endif //TFCS_WEAPON_BASE_H