#ifndef TFCS_WEAPON_BASE_H
#define TFCS_WEAPON_BASE_H
#ifdef _WIN32
#pragma once
#endif

#include "tfcs_weapon_parse.h"
#include "tfcs_shareddefs.h"

#ifdef CLIENT_DLL
#define CTFCSWeaponBase C_TFCSWeaponBase
#endif

class CTFCSWeaponBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS(CTFCSWeaponBase, CBaseCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponBase();
	~CTFCSWeaponBase();

#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	virtual void			Spawn(void);

	virtual bool			IsPredicted() const { return true; }
	
	virtual int				GetWeaponID() { Assert(0); return WEAPON_NONE; }

	CTFCSWeaponInfo const	&GetTFCSWpnData() const;

	//TODO get player owner

	virtual void			PrimaryAttack();

	virtual void			Drop(const Vector &vecVelocity);
	void					DropThink(void);

	virtual float			GetFOV() { return -1; }

	const char				*GetWorldModel(void) const;

private:

	CTFCSWeaponBase(const CTFCSWeaponBase &);

#ifdef CLIENT_DLL
	virtual bool			ShouldPretict();
	virtual void			OnDataChanged(DataUpdateType_t type);
	virtual int				DrawModel(int flags);
	virtual RenderGroup_t	GetRenderGroup(void);
	virtual ShadowType_t	ShadowCastType(void);
#endif
};
#endif //TFCS_WEAPON_BASE_H