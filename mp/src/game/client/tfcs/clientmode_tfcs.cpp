#include "cbase.h"
#include "clientmode_tfcs.h"
#include "c_weapon__stubs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

STUB_WEAPON_CLASS( cycler_weapon, WeaponCycler, C_BaseCombatWeapon );
STUB_WEAPON_CLASS( weapon_cubemap, WeaponCubemap, C_BaseCombatWeapon );

ConVar fov_desired( "fov_desired", "75", FCVAR_ARCHIVE | FCVAR_USERINFO, "Sets the base field-of-view.", true, 75.0, true, 90.0 );
ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

// The current client mode. Always ClientModeNormal in HL.
IClientMode *g_pClientMode = NULL;

// Instance the singleton and expose the interface to it.
IClientMode *GetClientModeNormal()
{
	static ClientModeTFCS g_ClientModeNormal;
	return &g_ClientModeNormal;
}

//-----------------------------------------------------------------------------
// CTFCSModeManager implementation
//-----------------------------------------------------------------------------
void CTFCSModeManager::Init( void )
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CTFCSModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

void CTFCSModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

static CTFCSModeManager g_TFCSModeManager;
IVModeManager *modemanager = &g_TFCSModeManager;

//-----------------------------------------------------------------------------
// CHudViewport implementation
//-----------------------------------------------------------------------------
void CHudViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// ClientModeTFCS implementation
//-----------------------------------------------------------------------------
ClientModeTFCS::ClientModeTFCS()
{
	m_pViewport = new CHudViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeTFCS::~ClientModeTFCS()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientModeTFCS::Init()
{
	BaseClass::Init();
}