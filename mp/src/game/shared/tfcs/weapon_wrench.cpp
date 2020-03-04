#include "cbase.h"
#include "tfcs_weapon_base_melee.h"

#if defined( CLIENT_DLL )
	#define CTFCSWeaponSpanner C_TFCSWeaponSpanner
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
#endif


class CTFCSWeaponSpanner : public CTFCSWeaponBaseMelee
{
public:
	DECLARE_CLASS( CTFCSWeaponSpanner, CTFCSWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFCSWeaponSpanner();

	virtual int GetWeaponID() { return WEAPON_WRENCH; }

private:

	CTFCSWeaponSpanner( const CTFCSWeaponSpanner & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponSpanner, DT_TFCSWeaponSpanner )

BEGIN_NETWORK_TABLE( CTFCSWeaponSpanner, DT_TFCSWeaponSpanner )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponSpanner )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_wrench, CTFCSWeaponSpanner );
PRECACHE_WEAPON_REGISTER( weapon_wrench );

CTFCSWeaponSpanner::CTFCSWeaponSpanner()
{
}