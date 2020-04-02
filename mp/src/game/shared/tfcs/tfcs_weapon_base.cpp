//***************************************************************
//
//	TFCS Weapon Base Code
//
//	CTFCSWeaponBase
//	|
//	|--> CTFCSWeaponBaseMelee
//	|		|
//	|		|--> CTFCSWeaponCrowbar
//	|		|--> CTFCSWeaponMedikit
//	|		|--> CTFCSWeaponSpanner
//	|		|--> CTFCSWeaponUmbrella
//	|		|--> CTFCSWeaponKnife
//	|
//	|--> CTFCSWeaponBaseGrenade
//	|		|
//	|		|--> Rest of the throwable nades
//	|
//	|--> CTFWeaponBaseGun
//	|		|
//	|		|--> -CTFCSWeaponShotgun
//	|		|--> -CTFCSWeaponRailgun
//	|		|--> -CTFCSWeaponAutorifle
//	|		|--> -CTFCSWeaponTranq
//	|		|--> -CTFCSWeaponSuperShotgun
//	|		|--> -CTFCSWeaponSniperrifle
//	|		|--> -CTFCSWeaponFlamethrower
//	|		|--> -CTFCSWeaponNailgun
//	|		|--> -CTFCSWeaponSuperNailgun
//	|		|--> -CTFCSWeaponGrenadeLauncher
//	|		|--> -CTFCSWeaponRocketLauncher
//	|		|--> -CTFCSWeaponPipebombLauncher
//	|		|--> -CTFCSWeaponAssaultCannon
//	|		|--> -CTFCSWeaponIncindiaryCannon
//
//***************************************************************

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "ammodef.h"
#include "tfcs_weapon_base.h"

#ifdef CLIENT_DLL
	#include "c_tfcs_player.h"
#else
	#include "tfcs_player.h"
	#include "vphysics/constraints.h"
#endif

//Data tables
IMPLEMENT_NETWORKCLASS_ALIASED( TFCSWeaponBase, DT_TFCSWeaponBase )
	BEGIN_NETWORK_TABLE( CTFCSWeaponBase, DT_TFCSWeaponBase )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCSWeaponBase )
END_PREDICTION_DATA()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFCSWeaponBase )
	//DEFINE_ENTITYFUNC( DropThink ),
END_DATADESC()
#endif

// ----------------------------------------------------------------------------- //
// Global functions.
// ----------------------------------------------------------------------------- //
bool IsAmmoType( int iAmmoType, const char *pAmmoName )
{
	return GetAmmoDef()->Index( pAmmoName ) == iAmmoType;
}

CTFCSWeaponBase::CTFCSWeaponBase()
{
	SetPredictionEligible( true );
	AddSolidFlags( FSOLID_TRIGGER );

	m_bFiresUnderwater = true;
	//m_bMuzzleFlash = true;
	m_flNextResetCheckTime = 0.0f;

	SetCollisionGroup( COLLISION_GROUP_WEAPON );
}

CTFCSWeaponBase::~CTFCSWeaponBase()
{
}

void CTFCSWeaponBase::Spawn( void )
{
	BaseClass::Spawn();

	// Set this here to allow players to shoot dropped weapons
	SetCollisionGroup( COLLISION_GROUP_WEAPON );
}

//Tony; override for animation purposes.
bool CTFCSWeaponBase::Reload( void )
{
	bool fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
	if ( fRet )
	{
//		WeaponSound( RELOAD );
		ToTFCSPlayer(GetOwner())->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
	}
	return fRet;
}

void CTFCSWeaponBase::WeaponSound( WeaponSound_t sound_type, float soundtime /* = 0.0f */ )
{
#ifdef CLIENT_DLL

		// If we have some sounds from the weapon classname.txt file, play a random one of them
		const char *shootsound = GetWpnData().aShootSounds[ sound_type ]; 
		if ( !shootsound || !shootsound[0] )
			return;

		CBroadcastRecipientFilter filter; // this is client side only
		if ( !te->CanPredict() )
			return;
				
		CBaseEntity::EmitSound( filter, GetPlayerOwner()->entindex(), shootsound, &GetPlayerOwner()->GetAbsOrigin() ); 
#else
		BaseClass::WeaponSound( sound_type, soundtime );
#endif
}

CBasePlayer* CTFCSWeaponBase::GetPlayerOwner() const
{
	return dynamic_cast< CBasePlayer* >( GetOwner() );
}

CTFCSPlayer* CTFCSWeaponBase::GetTFCSPlayerOwner() const
{
	return ToTFCSPlayer( GetOwner() );
}

const CTFCSWeaponInfo &CTFCSWeaponBase::GetTFCSWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CTFCSWeaponInfo *pInfo = static_cast<const CTFCSWeaponInfo *>( pWeaponInfo );

	return *pInfo;
}

void CTFCSWeaponBase::PrimaryAttack( void )
{
	BaseClass::PrimaryAttack();
}

#ifdef CLIENT_DLL
bool CTFCSWeaponBase::ShouldPredict()
{
	if( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
		return true;

	return BaseClass::ShouldPredict();
}

void CTFCSWeaponBase::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( GetPredictable() && !ShouldPredict() )
		ShutdownPredictable();
}
#endif

#ifdef GAME_DLL
void CTFCSWeaponBase::Materialize( void )
{
	if ( IsEffectActive( EF_NODRAW ) )
	{
		// changing from invisible state to visible.
		EmitSound( "AlyxEmp.Charge" );
		
		RemoveEffects( EF_NODRAW );
		DoMuzzleFlash();
	}

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false );
		SetMoveType( MOVETYPE_VPHYSICS );

		TFCSGameRules()->AddLevelDesignerPlacedObject( this );
	}

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		if ( GetOriginalSpawnOrigin() == vec3_origin )
		{
			m_vOriginalSpawnOrigin = GetAbsOrigin();
			m_vOriginalSpawnAngles = GetAbsAngles();
		}
	}

	SetPickupTouch();

	SetThink (NULL);
}

int CTFCSWeaponBase::ObjectCaps()
{
	return BaseClass::ObjectCaps() & ~FCAP_IMPULSE_USE;
}

void CTFCSWeaponBase::FallInit( void )
{
	SetModel( GetWorldModel() );
	VPhysicsDestroyObject();

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		SetMoveType( MOVETYPE_NONE );
		SetSolid( SOLID_BBOX );
		AddSolidFlags( FSOLID_TRIGGER );

		UTIL_DropToFloor( this, MASK_SOLID );
	}
	else
	{
		if ( !VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false ) )
		{
			SetMoveType( MOVETYPE_NONE );
			SetSolid( SOLID_BBOX );
			AddSolidFlags( FSOLID_TRIGGER );
		}
		else
		{
			// Constrained start?
			if ( HasSpawnFlags( SF_WEAPON_START_CONSTRAINED ) )
			{
				//Constrain the weapon in place
				IPhysicsObject *pReferenceObject, *pAttachedObject;
				
				pReferenceObject = g_PhysWorldObject;
				pAttachedObject = VPhysicsGetObject();

				if ( pReferenceObject && pAttachedObject )
				{
					constraint_fixedparams_t fixed;
					fixed.Defaults();
					fixed.InitWithCurrentObjectState( pReferenceObject, pAttachedObject );
					
					fixed.constraint.forceLimit	= lbs2kg( 10000 );
					fixed.constraint.torqueLimit = lbs2kg( 10000 );

					IPhysicsConstraint *pConstraint = GetConstraint();

					pConstraint = physenv->CreateFixedConstraint( pReferenceObject, pAttachedObject, NULL, fixed );

					pConstraint->SetGameData( (void *) this );
				}
			}
		}
	}

	SetPickupTouch();
	
	SetThink( &CTFCSWeaponBase::FallThink );

	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CTFCSWeaponBase::FallThink( void )
{
	// Prevent the common HL2DM weapon respawn bug from happening
	// When a weapon is spawned, the following chain of events occurs:
	// - Spawn() is called (duh), which then calls FallInit()
	// - FallInit() is called, and prepares the weapon's 'Think' function (CBaseCombatWeapon::FallThink())
	// - FallThink() is called, and performs several checks before deciding whether the weapon should Materialize()
	// - Materialize() is called (the HL2DM version above), which sets the weapon's respawn location.
	// The problem occurs when a weapon isn't placed properly by a level designer.
	// If the weapon is unable to move from its location (e.g. if its bounding box is halfway inside a wall), Materialize() never gets called.
	// Since Materialize() never gets called, the weapon's respawn location is never set, so if a person picks it up, it respawns forever at
	// 0 0 0 on the map (infinite loop of fall, wait, respawn, not nice at all for performance and bandwidth!)
	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		if ( GetOriginalSpawnOrigin() == vec3_origin )
		{
			m_vOriginalSpawnOrigin = GetAbsOrigin();
			m_vOriginalSpawnAngles = GetAbsAngles();
		}
	}

	return BaseClass::FallThink();
}
#endif // GAME_DLL