#include "cbase.h"
#include "tfcs_weapon_base_melee.h"

#if defined( CLIENT_DLL )
	#define CTFCSWeaponMedikit C_TFCSWeaponMedikit
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
#endif

class CTFCSWeaponMedikit : public CTFCSWeaponBaseMelee
{
public:
	DECLARE_CLASS( CTFCSWeaponMedikit, CTFCSWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponMedikit();

	virtual int GetWeaponID() { return WEAPON_MEDIKIT; }

private:

	CTFCSWeaponMedikit( const CTFCSWeaponMedikit & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponMedikit, DT_TFCSWeaponMedikit )

BEGIN_NETWORK_TABLE( CTFCSWeaponMedikit, DT_TFCSWeaponMedikit )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponMedikit )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_medkit, CTFCSWeaponMedikit );
PRECACHE_WEAPON_REGISTER( weapon_medkit );

CTFCSWeaponMedikit::CTFCSWeaponMedikit()
{
}