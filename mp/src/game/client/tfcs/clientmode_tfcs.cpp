#include "cbase.h"
#include "hud.h"
#include "clientmode_tfcs.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include "vgui/isurface.h"
#include "vgui/ipanel.h"
#include <vgui_controls/AnimationController.h>
#include "BuyMenu.h"
#include "filesystem.h"
#include "vgui/ivgui.h"
#include "iinput.h"
#include <vgui/IInput.h>
#include "hud_basechat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <keyvalues.h>
#include "text_message.h"
#include "tfcs_weapon_base.h"
#include "c_tfcs_player.h"
#include "c_team.h"
//#include "c_tfcs_team.h"
//#include "c_tfcs_player_resource.h"
#include "c_weapon__stubs.h"		//Tony; add stubs
#include "hud_vote.h"
#include "cam_thirdperson.h"
#include "tfcs_hud_chat.h"

#ifdef GLOWS_ENABLE
#include "glow_outline_effect.h"
#include "clienteffectprecachesystem.h"
#endif

class CHudChat;
class CHudVote;

static bool s_bFixedInputActive = false;
static int s_nOldCursor[2] = { 0, 0 };

bool PlayerNameNotSetYet( const char *pszName );

ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );
ConVar fov_desired( "fov_desired", "90", FCVAR_ARCHIVE | FCVAR_USERINFO, "Sets the base field-of-view.", true, 75.0, true, 130.0 );

extern ConVar v_viewmodel_fov;

IClientMode *g_pClientMode = NULL;

//Tony; add stubs for cycler weapon and cubemap.
STUB_WEAPON_CLASS( cycler_weapon,   WeaponCycler,   C_BaseCombatWeapon );
STUB_WEAPON_CLASS( weapon_cubemap,  WeaponCubemap,  C_BaseCombatWeapon );

// this set of convars's purpose is to shut up the console
ConVar hud_saytext( "hud_saytext", "0", FCVAR_HIDDEN );
ConVar spec_drawstatus( "spec_drawstatus", "0", FCVAR_HIDDEN );

//-----------------------------------------------------------------------------
// HACK: the detail sway convars are archive, and default to 0.  Existing CS:S players thus have no detail
// prop sway.  We'll force them to DoD's default values for now.  What we really need in the long run is
// a system to apply changes to archived convars' defaults to existing players.
//-----------------------------------------------------------------------------
extern ConVar cl_detail_max_sway;
extern ConVar cl_detail_avoid_radius;
extern ConVar cl_detail_avoid_force;
extern ConVar cl_detail_avoid_recover_speed;

#ifdef GLOWS_ENABLE
CLIENTEFFECT_REGISTER_BEGIN( PrecachePostProcessingEffectsGlow )
	CLIENTEFFECT_MATERIAL( "dev/glow_blur_x" )
	CLIENTEFFECT_MATERIAL( "dev/glow_blur_y" )
	CLIENTEFFECT_MATERIAL( "dev/glow_color" )
	CLIENTEFFECT_MATERIAL( "dev/glow_downsample" )
	CLIENTEFFECT_MATERIAL( "dev/halo_add_to_screen" )
CLIENTEFFECT_REGISTER_END_CONDITIONAL(	engine->GetDXSupportLevel() >= 90 )
#endif

static CTFCSModeManager g_ModeManager;
IVModeManager *modemanager = ( IVModeManager * )&g_ModeManager;

//-----------------------------------------------------------------------------
// Purpose: CTFCSModeManager implementation.
//-----------------------------------------------------------------------------
void CTFCSModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();
	
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );

	g_ThirdPersonManager.Init();

	// HACK: the detail sway convars are archive, and default to 0.  Existing CS:S players thus have no detail
	// prop sway.  We'll force them to DoD's default values for now.
	if ( !cl_detail_max_sway.GetFloat() &&
		!cl_detail_avoid_radius.GetFloat() &&
		!cl_detail_avoid_force.GetFloat() &&
		!cl_detail_avoid_recover_speed.GetFloat() )
	{
		cl_detail_max_sway.SetValue( "5" );
		cl_detail_avoid_radius.SetValue( "64" );
		cl_detail_avoid_force.SetValue( "0.4" );
		cl_detail_avoid_recover_speed.SetValue( "0.25" );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCSModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeTFCS::ClientModeTFCS()
{
}

//-----------------------------------------------------------------------------
// Purpose: If you don't know what a destructor is by now, you are probably going to get fired
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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ClientModeTFCS::InitViewport()
{
	m_pViewport = new TFCSViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

ClientModeTFCS g_ClientModeNormal;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
IClientMode *GetClientModeNormal()
{
	return &g_ClientModeNormal;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
ClientModeTFCS* GetClientModeTFCS()
{
	Assert( dynamic_cast< ClientModeTFCS* >( GetClientModeNormal() ) );

	return static_cast< ClientModeTFCS* >( GetClientModeNormal() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float ClientModeTFCS::GetViewModelFOV( void )
{
	//Tony; retrieve the fov from the view model script, if it overrides it.
	float viewFov = v_viewmodel_fov.GetFloat();

	/*auto *pWeapon = (CTFCSWeaponBase*)GetActiveWeapon();
	if ( pWeapon )
		viewFov = pWeapon->GetWeaponFOV();*/

	return viewFov;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ClientModeTFCS::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool ClientModeTFCS::DoPostScreenSpaceEffects( const CViewSetup *pSetup )
{
#ifdef GLOWS_ENABLE
	CMatRenderContextPtr pRenderContext( materials );

	pRenderContext->PushRenderTargetAndViewport();
	g_GlowObjectManager.RenderGlowEffects( pSetup );
	pRenderContext->PopRenderTargetAndViewport();

	pRenderContext.SafeRelease();
#endif

	return BaseClass::DoPostScreenSpaceEffects( pSetup );
}

//-----------------------------------------------------------------------------
// Purpose: let us drive the fixed camera position around
//-----------------------------------------------------------------------------
void Handle_Fixed_Input( bool active )
{
	if ( s_bFixedInputActive ^ active )
	{
		if ( s_bFixedInputActive && !active )
			// Restore mouse
			vgui::input()->SetCursorPos( s_nOldCursor[0], s_nOldCursor[1] );
		else
			// todO: set the initial fixed vectors here..
			vgui::input()->GetCursorPos( s_nOldCursor[0], s_nOldCursor[1] );
	}

	if ( active )
	{
		int mx, my;
		int dx, dy;

		vgui::input()->GetCursorPos( mx, my );

		dx = mx - s_nOldCursor[0];
		dy = my - s_nOldCursor[1];

		vgui::input()->SetCursorPos( s_nOldCursor[0], s_nOldCursor[1] );
	}

	s_bFixedInputActive = active;
}

//-----------------------------------------------------------------------------
// Purpose: Fixes some bugs from base class.
//-----------------------------------------------------------------------------
void ClientModeTFCS::OverrideView( CViewSetup *pSetup )
{
	QAngle camAngles;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	// Let the player override the view.
	pPlayer->OverrideView( pSetup );

	if ( ::input->CAM_IsThirdPerson() && !pPlayer->IsObserver() )
	{
		const Vector& cam_ofs = g_ThirdPersonManager.GetCameraOffsetAngles();
		Vector cam_ofs_distance;

		if ( g_ThirdPersonManager.IsOverridingThirdPerson() )
			cam_ofs_distance = g_ThirdPersonManager.GetDesiredCameraOffset();
		else
			cam_ofs_distance = g_ThirdPersonManager.GetFinalCameraOffset();

		cam_ofs_distance *= g_ThirdPersonManager.GetDistanceFraction();

		camAngles[PITCH] = cam_ofs[PITCH];
		camAngles[YAW] = cam_ofs[YAW];
		camAngles[ROLL] = 0;

		Vector camForward, camRight, camUp;

		if ( g_ThirdPersonManager.IsOverridingThirdPerson() == false )
			engine->GetViewAngles( camAngles );

		// get the forward vector
		AngleVectors( camAngles, &camForward, &camRight, &camUp );

		VectorMA( pSetup->origin, -cam_ofs_distance[0], camForward, pSetup->origin );
		VectorMA( pSetup->origin, cam_ofs_distance[1], camRight, pSetup->origin );
		VectorMA( pSetup->origin, cam_ofs_distance[2], camUp, pSetup->origin );

		// Override angles from third person camera
		VectorCopy( camAngles, pSetup->angles );
	}
	else if ( ::input->CAM_IsOrthographic() )
	{
		pSetup->m_bOrtho = true;
		float w, h;
		::input->CAM_OrthographicSize( w, h );
		w *= 0.5f;
		h *= 0.5f;
		pSetup->m_OrthoLeft = -w;
		pSetup->m_OrthoTop = -h;
		pSetup->m_OrthoRight = w;
		pSetup->m_OrthoBottom = h;
	}

	Handle_Fixed_Input( vgui::input()->IsKeyDown( KEY_LSHIFT ) && vgui::input()->IsMouseDown( MOUSE_LEFT ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientModeTFCS::FireGameEvent( IGameEvent *event )
{
	if ( Q_strcmp( "player_team", event->GetName() ) == 0 )
	{
		int userid = event->GetInt( "userid" );

		// We have to know if this is the local player or not.
		// Various systems depend on knowing what team we are changing to.
		bool bIsLocalPlayer = false;

		auto* pPlayer = USERID2PLAYER( userid );
		if ( !pPlayer )
		{
			int iLocalPlayer = engine->GetLocalPlayer();
			if ( iLocalPlayer > 0 )
			{
				player_info_s info;
				engine->GetPlayerInfo( iLocalPlayer, &info );

				bIsLocalPlayer = userid == info.userID;
			}
		}

		bool bDisconnected = event->GetBool( "disconnect" );
		if ( bDisconnected )
			return;

		int team = event->GetInt( "team" );
		bool bAutoTeamed = event->GetInt( "autoteam", false );
		bool bSilent = event->GetInt( "silent", false );
		const char* pszName = event->GetString( "name" );

		//
		// Print to chat
		//
		CHudChat* pHudChat = GetTFCChatHud();
		if ( !bSilent && pHudChat && !PlayerNameNotSetYet( pszName ) )
		{
			wchar_t wszPlayerName[MAX_PLAYER_NAME_LENGTH];
			g_pVGuiLocalize->ConvertANSIToUnicode( pszName, wszPlayerName, sizeof(wszPlayerName) );

			wchar_t wszTeam[64];
			C_Team *pTeam = GetGlobalTeam( team );
			if ( pTeam )
				g_pVGuiLocalize->ConvertANSIToUnicode( pTeam->Get_Name(), wszTeam, sizeof(wszTeam) );
			else
				_snwprintf ( wszTeam, sizeof( wszTeam ) / sizeof( wchar_t ), L"%d", team );
			
			wchar_t wszLocalized[100];
			if ( bAutoTeamed )
				g_pVGuiLocalize->ConstructString( wszLocalized, sizeof( wszLocalized ), g_pVGuiLocalize->Find( "#game_player_joined_autoteam" ), 2, wszPlayerName, wszTeam );
			else
				g_pVGuiLocalize->ConstructString( wszLocalized, sizeof( wszLocalized ), g_pVGuiLocalize->Find( "#game_player_joined_team" ), 2, wszPlayerName, wszTeam );

			char szLocalized[100];
			g_pVGuiLocalize->ConvertUnicodeToANSI( wszLocalized, szLocalized, sizeof(szLocalized) );

			pHudChat->Printf( CHAT_FILTER_TEAMCHANGE, "%s", szLocalized );
		}


		//
		// Fire local player team change methods.
		// If our local player doesn't exist yet, call static method instead.
		//
		if ( pPlayer && pPlayer->IsLocalPlayer() )
			pPlayer->TeamChange( team ); // that's other guy.
		else if ( bIsLocalPlayer )
			C_TFCSPlayer::TeamChangeStatic( team ); // that's me.

		return;
	}

	BaseClass::FireGameEvent( event );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool ClientModeTFCS::CanRecordDemo( char *errorMsg, int length ) const
{
	auto *player = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( !player )
		return true;

	if ( !player->IsAlive() )
		return true;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: We've received a keypress from the engine. Return 1 if the engine is allowed to handle it.
//-----------------------------------------------------------------------------
int	ClientModeTFCS::KeyInput( int down, ButtonCode_t keynum, const char *pszCurrentBinding )
{
	// If we're voting...
	CHudVote *pHudVote = GET_HUDELEMENT( CHudVote );
	if ( pHudVote && pHudVote->IsVisible() )
	{
		if ( !pHudVote->KeyInput( down, keynum, pszCurrentBinding ) )
			return 0;
	}

	return BaseClass::KeyInput( down, keynum, pszCurrentBinding );
}