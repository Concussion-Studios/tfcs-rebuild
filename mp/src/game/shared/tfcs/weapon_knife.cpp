#include "cbase.h"
#include "tfcs_weapon_base_melee.h"

#if defined( CLIENT_DLL )
	#define CTFCSWeaponKnife C_TFCSWeaponKnife
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
#endif

class CTFCSWeaponKnife : public CTFCSWeaponBaseMelee
{
public:
	DECLARE_CLASS( CTFCSWeaponKnife, CTFCSWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponKnife();

	virtual int GetWeaponID() { return WEAPON_KNIFE; }

private:

	CTFCSWeaponKnife( const CTFCSWeaponKnife & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponKnife, DT_TFCSWeaponKnife )

BEGIN_NETWORK_TABLE( CTFCSWeaponKnife, DT_TFCSWeaponKnife )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponKnife )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_knife, CTFCSWeaponKnife );
PRECACHE_WEAPON_REGISTER( weapon_knife );

CTFCSWeaponKnife::CTFCSWeaponKnife()
{
}