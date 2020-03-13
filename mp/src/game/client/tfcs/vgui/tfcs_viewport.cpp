//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#pragma warning( disable : 4800  )  // disable forcing int to bool performance warning

// VGUI panel includes
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui/Cursor.h>
#include <vgui/IScheme.h>
#include <vgui/IVGUI.h>
#include <vgui/ILocalize.h>
#include <vgui/VGUI.h>

// client dll/engine defines
#include "hud.h"
#include <voice_status.h>

// viewport definitions
#include <baseviewport.h>
#include "tfcs_viewport.h"

#include "vguicenterprint.h"
#include "text_message.h"
#include "c_tfcs_player.h"
#include "tfcs_scoreboard.h"
#include "tfcs_textwindow.h"
#include "tfcs_spectatorgui.h"
//#include "tfcs_classmenu.h"
#include "tfcs_teammenu.h"

/*CON_COMMAND_F( changeteam, "Choose a new team", FCVAR_SERVER_CAN_EXECUTE|FCVAR_CLIENTCMD_CAN_EXECUTE )
{
	C_TFCSPlayer *pPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( pPlayer && pPlayer->CanShowTeamMenu() )
		gViewPortInterface->ShowPanel( PANEL_TEAM, true );
}

CON_COMMAND_F( changeclass, "Choose a new class", FCVAR_SERVER_CAN_EXECUTE|FCVAR_CLIENTCMD_CAN_EXECUTE )
{
	C_TFCSPlayer *pPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();

	if ( pPlayer && pPlayer->CanShowClassMenu())
	{
		switch( pPlayer->GetTeamNumber() )
		{
		case TEAM_BLUE:
			gViewPortInterface->ShowPanel( PANEL_CLASS_BLUE, true );
			break;
		case TEAM_RED:
			gViewPortInterface->ShowPanel( PANEL_CLASS_RED, true );
			break;
		case TEAM_GREEN:
			gViewPortInterface->ShowPanel( PANEL_CLASS_GREEN, true );
			break;
		case TEAM_YELLOW:
			gViewPortInterface->ShowPanel( PANEL_CLASS_YELLOW, true );
			break;
		default:
			break;
		}
	}
}*/

CON_COMMAND_F( spec_help, "Show spectator help screen", FCVAR_CLIENTCMD_CAN_EXECUTE)
{
	if ( gViewPortInterface )
		gViewPortInterface->ShowPanel( PANEL_INFO, true );
}

CON_COMMAND_F( spec_menu, "Activates spectator menu", FCVAR_CLIENTCMD_CAN_EXECUTE)
{
	bool bShowIt = true;

	C_TFCSPlayer *pPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( pPlayer && !pPlayer->IsObserver() )
		return;

	if ( args.ArgC() == 2 )
		 bShowIt = atoi( args[ 1 ] ) == 1;
	
	if ( gViewPortInterface )
		gViewPortInterface->ShowPanel( PANEL_SPECMENU, bShowIt );
}

CON_COMMAND_F( togglescores, "Toggles score panel", FCVAR_CLIENTCMD_CAN_EXECUTE)
{
	if ( !gViewPortInterface )
		return;
	
	IViewPortPanel *scoreboard = gViewPortInterface->FindPanelByName( PANEL_SCOREBOARD );

	if ( !scoreboard )
		return;

	if ( scoreboard->IsVisible() )
	{
		gViewPortInterface->ShowPanel( scoreboard, false );
		GetClientVoiceMgr()->StopSquelchMode();
	}
	else
	{
		gViewPortInterface->ShowPanel( scoreboard, true );
	}
}

void TFCSViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}


IViewPortPanel* TFCSViewport::CreatePanelByName( const char *szPanelName )
{
	IViewPortPanel* newpanel = NULL;

	if ( Q_strcmp( PANEL_SCOREBOARD, szPanelName) == 0 )
		newpanel = new CTFCSScoreboard( this );
	else if ( Q_strcmp( PANEL_INFO, szPanelName) == 0 )
		newpanel = new CTFCSTextWindow( this );
	else if ( Q_strcmp(PANEL_SPECGUI, szPanelName) == 0 )
		newpanel = new CTFCSSpectatorGUI( this );	
	/*else if ( Q_strcmp( PANEL_CLASS_BLUE, szPanelName) == 0 )
		newpanel = new CTFCSClassMenu_Blue( this );
	else if ( Q_strcmp( PANEL_CLASS_RED, szPanelName) == 0 )
		newpanel = new CTFCSClassMenu_Red( this );
	else if ( Q_strcmp( PANEL_CLASS_GREEN, szPanelName) == 0 )
		newpanel = new CTFCSClassMenu_Green( this );
	else if ( Q_strcmp( PANEL_CLASS_YELLOW, szPanelName) == 0 )
		newpanel = new CTFCSClassMenu_Yellow( this );*/
	else if ( Q_strcmp( PANEL_TEAM, szPanelName) == 0 )
		newpanel = new CTFCSTeamMenu( this );
	else
		// create a generic base panel, don't add twice
		newpanel = BaseClass::CreatePanelByName( szPanelName );

	return newpanel; 
}

void TFCSViewport::CreateDefaultPanels( void )
{
	//AddNewPanel( CreatePanelByName( PANEL_CLASS_BLUE ), "PANEL_CLASS_BLUE" );
	//AddNewPanel( CreatePanelByName( PANEL_CLASS_RED ), "PANEL_CLASS_RED" );
	//AddNewPanel( CreatePanelByName( PANEL_CLASS_GREEN ), "PANEL_CLASS_GREEN" );
	//AddNewPanel( CreatePanelByName( PANEL_CLASS_YELLOW ), "PANEL_CLASS_YELLOW" );
	AddNewPanel( CreatePanelByName( PANEL_TEAM ), "PANEL_TEAM" );

	BaseClass::CreateDefaultPanels();
}

int TFCSViewport::GetDeathMessageStartHeight( void )
{
	int x = YRES(2);

	IViewPortPanel *spectator = gViewPortInterface->FindPanelByName( PANEL_SPECGUI );

	//TODO: Link to actual height of spectator bar
	if ( spectator && spectator->IsVisible() )
		x += YRES(52);

	return x;
}