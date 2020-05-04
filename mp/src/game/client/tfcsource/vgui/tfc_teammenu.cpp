//======== Copyright © 1996-2008, Valve Corporation, All rights reserved. =========//
//
// Purpose: 
//
// $NoKeywords: $
//=================================================================================//
#include "cbase.h"
#include <stdio.h>
#include <cdll_client_int.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/ImageList.h>
#include <FileSystem.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/RichText.h>
#include "cdll_util.h"
#include <game/client/iviewport.h>
#include "tfc_backgroundpanel.h"
#include "tfc_teammenu.h"
#include "tfc_gamerules.h"
#include "c_tfc_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCTeamMenu::CTFCTeamMenu( IViewPort *pViewPort ) : CTeamMenu( pViewPort )
{
	CreateBackground( this );
	m_backgroundLayoutFinished = false;

	/*if ( TFCGameRules() && TFCGameRules()->IsTDMGamemode() )
		LoadControlSettings( "Resource/UI/4TeamsMenu.res" );
	else*/
		LoadControlSettings( "Resource/UI/TeamMenu.res" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCTeamMenu::~CTFCTeamMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: called to update the menu with new information
//-----------------------------------------------------------------------------
void CTFCTeamMenu::Update( void )
{
	BaseClass::Update();

	const ConVar *allowspecs =  cvar->FindVar( "mp_allowspectators" );

	C_TFCPlayer *pPlayer = C_TFCPlayer::GetLocalTFCPlayer();
	if ( !pPlayer || !TFCGameRules() )
		return;

	if ( allowspecs && allowspecs->GetBool() )
	{
		if ( pPlayer->GetTeamNumber() == TEAM_UNASSIGNED || ( pPlayer && pPlayer->IsPlayerDead() ) )
			SetVisibleButton("specbutton", true);
		else
			SetVisibleButton("specbutton", true);
	}
	else
		SetVisibleButton("specbutton", false );

	if( pPlayer->GetTeamNumber() == TEAM_UNASSIGNED ) // we aren't on a team yet
		SetVisibleButton("CancelButton", false); 
	else
		SetVisibleButton("CancelButton", true); 

	//if( TFCGameRules() && TFCGameRules()->IsTDMGamemode() ) // we don't had 4teams on
	{
		SetVisibleButton("yellowbutton", false);
		SetVisibleButton("greenbutton", false);
	}
	/*else
	{
		SetVisibleButton("yellowbutton", true);
		SetVisibleButton("greenbutton", true);
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCTeamMenu::SetVisible( bool state )
{
	BaseClass::SetVisible(state);

	if ( state )
	{
		Panel *pAutoButton = FindChildByName( "autobutton" );
		if ( pAutoButton )
			pAutoButton->RequestFocus();
	}
}

//-----------------------------------------------------------------------------
// Purpose: When a team button is pressed it triggers this function to 
//			cause the player to join a team
//-----------------------------------------------------------------------------
void CTFCTeamMenu::OnCommand( const char *command )
{
	if ( Q_stricmp( command, "vguicancel" ) )
		engine->ClientCmd( command );
	
	BaseClass::OnCommand( command );

	gViewPortInterface->ShowBackGround( false );
	OnClose();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the visibility of a button by name
//-----------------------------------------------------------------------------
void CTFCTeamMenu::SetVisibleButton( const char *textEntryName, bool state )
{
	Button *entry = dynamic_cast<Button *>( FindChildByName( textEntryName ) );
	if ( entry )
		entry->SetVisible( state );
}

//-----------------------------------------------------------------------------
// Purpose: Scale / center the window
//-----------------------------------------------------------------------------
void CTFCTeamMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	// stretch the window to fullscreen
	if ( !m_backgroundLayoutFinished )
		LayoutBackgroundPanel( this );
	m_backgroundLayoutFinished = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCTeamMenu::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	ApplyBackgroundSchemeSettings( this, pScheme );
}