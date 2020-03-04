#include "cbase.h"
#include "tfcs_weapon_base_melee.h"

#if defined( CLIENT_DLL )
	#define CTFCSWeaponCrowbar C_TFCSWeaponCrowbar
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
#endif


class CTFCSWeaponCrowbar : public CTFCSWeaponBaseMelee
{
public:
	DECLARE_CLASS( CTFCSWeaponCrowbar, CTFCSWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CTFCSWeaponCrowbar();

	virtual int GetWeaponID() { return WEAPON_CROWBAR; }

private:

	CTFCSWeaponCrowbar( const CTFCSWeaponCrowbar & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponCrowbar, DT_TFCSWeaponCrowbar )

BEGIN_NETWORK_TABLE( CTFCSWeaponCrowbar, DT_TFCSWeaponCrowbar )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponCrowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_crowbar, CTFCSWeaponCrowbar );
PRECACHE_WEAPON_REGISTER( weapon_crowbar );

CTFCSWeaponCrowbar::CTFCSWeaponCrowbar()
{
}

