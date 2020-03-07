#include "cbase.h"
#include "tfcs_weapon_base_melee.h"

#if defined( CLIENT_DLL )
	#define CTFCSWeaponUmbrella C_TFCSWeaponUmbrella
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
#endif

class CTFCSWeaponUmbrella : public CTFCSWeaponBase
{
public:
	DECLARE_CLASS( CTFCSWeaponUmbrella, CTFCSWeaponBase );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponUmbrella();

	virtual int GetWeaponID() { return WEAPON_UMBRELLA; }

private:

	CTFCSWeaponUmbrella( const CTFCSWeaponUmbrella & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponUmbrella, DT_TFCSWeaponUmbrella )

BEGIN_NETWORK_TABLE( CTFCSWeaponUmbrella, DT_TFCSWeaponUmbrella )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponUmbrella )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_umbrella, CTFCSWeaponUmbrella );
PRECACHE_WEAPON_REGISTER( weapon_umbrella);

CTFCSWeaponUmbrella::CTFCSWeaponUmbrella()
{
}