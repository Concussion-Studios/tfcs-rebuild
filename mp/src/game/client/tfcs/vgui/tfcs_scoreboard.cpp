//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "tfcs_scoreboard.h"
#include "c_team.h"
#include "c_playerresource.h"
#include "c_tfcs_player.h"
//#include "c_tfcs_team.h"
#include "tfcs_backgroundpanel.h"
#include "tfcs_gamerules.h"
//#include "c_tfcs_player_resource.h"

#include <KeyValues.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVgui.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/SectionedListPanel.h>

#include "voice_status.h"
#include "vgui_avatarimage.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFCSScoreboard::CTFCSScoreboard( IViewPort *pViewPort ) : CClientScoreBoardDialog( pViewPort )
{
	m_iImageDead = 0;

	m_pPlayerListRed = new SectionedListPanel( this, "PlayerListRed" );
	m_pPlayerCountLabel_Red = new Label( this, "Red_PlayerCount", "" );
	m_pScoreLabel_Red = new Label( this, "Red_Score", "" );
	m_pPingLabel_Red = new Label( this, "Red_Latency", "" );

	m_pPlayerListBlue = new SectionedListPanel( this, "PlayerListBlue" );
	m_pPlayerCountLabel_Blue = new Label( this, "Blue_PlayerCount", "" );
	m_pScoreLabel_Blue = new Label( this, "Blue_Score", "" );
	m_pPingLabel_Blue = new Label( this, "Blue_Latency", "" );

	m_pVertLine = new ImagePanel( this, "VerticalLine" );

	ListenForGameEvent( "server_spawn" );
	SetDialogVariable( "server", "" );
	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCSScoreboard::~CTFCSScoreboard()
{
}

//-----------------------------------------------------------------------------
// Purpose: Paint background for rounded corners
//-----------------------------------------------------------------------------
void CTFCSScoreboard::PaintBackground()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawRoundedBackground( m_bgColor, wide, tall );
}

//-----------------------------------------------------------------------------
// Purpose: Paint border for rounded corners
//-----------------------------------------------------------------------------
void CTFCSScoreboard::PaintBorder()
{
	int wide, tall;
	GetSize( wide, tall );

	DrawRoundedBorder( m_borderColor, wide, tall );
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CTFCSScoreboard::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "Resource/UI/ScoreBoard.res" );

	m_bgColor = GetSchemeColor( "SectionedListPanel.BgColor", GetBgColor(), pScheme );
	m_borderColor = pScheme->GetColor( "FgColor", Color( 0, 0, 0, 0 ) );

	SetBgColor( Color( 0, 0, 0, 0 ) );
	SetBorder( pScheme->GetBorder( "BaseBorder" ) );

	if ( m_pImageList )
	{
		m_iImageDead = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_dead", true ) );

		// resize the images to our resolution
		for (int i = 1; i < m_pImageList->GetImageCount(); i++ )
		{
			int wide = 13, tall = 13;
			m_pImageList->GetImage(i)->SetSize(scheme()->GetProportionalScaledValueEx( GetScheme(), wide ), scheme()->GetProportionalScaledValueEx( GetScheme(),tall ) );
		}
	}

	if ( m_pPlayerListRed )
	{
		m_pPlayerListRed->SetImageList( m_pImageList, false );
		m_pPlayerListRed->SetBgColor( Color( 0, 0, 0, 0 ) );
		m_pPlayerListRed->SetBorder( NULL );
		m_pPlayerListRed->SetVisible( false );
	}

	if ( m_pPlayerListBlue )
	{
		m_pPlayerListBlue->SetImageList( m_pImageList, false );
		m_pPlayerListBlue->SetBgColor( Color( 0, 0, 0, 0 ) );
		m_pPlayerListBlue->SetBorder( NULL );
		m_pPlayerListBlue->SetVisible( false );
	}

	// turn off the default player list since we have our own
	if ( m_pPlayerList )
		m_pPlayerList->SetVisible( false );

	m_pScoreHeader_Red = (Label*)FindChildByName( "Red_ScoreHeader" );
	m_pDeathsHeader_Red = (Label*)FindChildByName( "Red_DeathsHeader" );
	m_pPingHeader_Red = (Label*)FindChildByName( "Red_PingHeader" );

	if ( m_pPlayerCountLabel_Red && m_pScoreHeader_Red && m_pScoreLabel_Red && m_pDeathsHeader_Red && m_pPingHeader_Red && m_pPingLabel_Red )
	{
		m_pPlayerCountLabel_Red->SetFgColor( COLOR_RED );
		m_pScoreHeader_Red->SetFgColor( COLOR_RED );
		m_pScoreLabel_Red->SetFgColor( COLOR_RED );
		m_pDeathsHeader_Red->SetFgColor( COLOR_RED );
		m_pPingHeader_Red->SetFgColor( COLOR_RED );
		m_pPingLabel_Red->SetFgColor( COLOR_RED );
	}

	m_pScoreHeader_Blue = (Label*)FindChildByName( "Blue_ScoreHeader" );
	m_pDeathsHeader_Blue = (Label*)FindChildByName( "Blue_DeathsHeader" );
	m_pPingHeader_Blue = (Label*)FindChildByName( "Blue_PingHeader" );

	if ( m_pPlayerCountLabel_Blue && m_pScoreHeader_Blue && m_pScoreLabel_Blue && m_pDeathsHeader_Blue && m_pPingHeader_Blue && m_pPingLabel_Blue )
	{
		m_pPlayerCountLabel_Blue->SetFgColor( COLOR_BLUE );
		m_pScoreHeader_Blue->SetFgColor( COLOR_BLUE );
		m_pScoreLabel_Blue->SetFgColor( COLOR_BLUE );
		m_pDeathsHeader_Blue->SetFgColor( COLOR_BLUE );
		m_pPingHeader_Blue->SetFgColor( COLOR_BLUE );
		m_pPingLabel_Blue->SetFgColor( COLOR_BLUE );
	}

	// Store the scoreboard width, for Update();
	m_iStoredScoreboardWidth = GetWide();

	SetVisible( false );
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Resets the scoreboard panel
//-----------------------------------------------------------------------------
void CTFCSScoreboard::Reset()
{
	InitPlayerList( m_pPlayerListRed, TEAM_RED );
	InitPlayerList( m_pPlayerListBlue, TEAM_BLUE );
}

//-----------------------------------------------------------------------------
// Purpose: Used for sorting players
//-----------------------------------------------------------------------------
bool CTFCSScoreboard::SDKPlayerSortFunc( vgui::SectionedListPanel *list, int itemID1, int itemID2 )
{
	KeyValues *it1 = list->GetItemData( itemID1 );
	KeyValues *it2 = list->GetItemData( itemID2 );
	Assert( it1 && it2 );

	// first compare score
	int v1 = it1->GetInt( "frags" );
	int v2 = it2->GetInt( "frags" );
	if ( v1 > v2 )
		return true;
	else if ( v1 < v2 )
		return false;

	// second compare deaths
	v1 = it1->GetInt( "deaths" );
	v2 = it2->GetInt( "deaths" );
	if ( v1 > v2 )
		return false;
	else if ( v1 < v2 )
		return true;

	// if score and deaths are the same, use player index to get deterministic sort
	int iPlayerIndex1 = it1->GetInt( "playerIndex" );
	int iPlayerIndex2 = it2->GetInt( "playerIndex" );
	return ( iPlayerIndex1 > iPlayerIndex2 );
}

//-----------------------------------------------------------------------------
// Purpose: Inits the player list in a list panel
//-----------------------------------------------------------------------------
void CTFCSScoreboard::InitPlayerList( SectionedListPanel *pPlayerList, int teamNumber )
{
	pPlayerList->SetVerticalScrollbar( false );
	pPlayerList->RemoveAll();
	pPlayerList->RemoveAllSections();
	pPlayerList->AddSection( 0, "Players", SDKPlayerSortFunc );
	pPlayerList->SetSectionAlwaysVisible( 0, true );
	pPlayerList->SetSectionFgColor( 0, Color( 255, 255, 255, 255 ) );
	pPlayerList->SetBgColor( Color( 0, 0, 0, 0 ) );
	pPlayerList->SetBorder( NULL );

	// set the section to have the team color
	if ( teamNumber && GameResources() )
		pPlayerList->SetSectionFgColor( 0, GameResources()->GetTeamColor( teamNumber ) );

	if ( ShowAvatars() )
		pPlayerList->AddColumnToSection( 0, "avatar",	"", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_CENTER, m_iAvatarWidth );

	pPlayerList->AddColumnToSection( 0, "name",		"", 0, m_iNameWidth );
	pPlayerList->AddColumnToSection( 0, "status",	"", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_RIGHT, m_iStatusWidth );
	pPlayerList->AddColumnToSection( 0, "class",	"", 0, m_iClassWidth );
	pPlayerList->AddColumnToSection( 0, "score",	"", SectionedListPanel::COLUMN_RIGHT, m_iScoreWidth );
	pPlayerList->AddColumnToSection( 0, "frags",	"", SectionedListPanel::COLUMN_RIGHT, m_iFragsWidth );
	pPlayerList->AddColumnToSection( 0, "deaths",	"", SectionedListPanel::COLUMN_RIGHT, m_iDeathWidth );
	pPlayerList->AddColumnToSection( 0, "ping",		"", SectionedListPanel::COLUMN_RIGHT, m_iPingWidth );
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFCSScoreboard::Update()
{
	UpdateItemVisibiity();
	UpdateTeamInfo();
	UpdatePlayerList();
	UpdateSpectatorList();
	MoveToCenterOfScreen();

	// update every second
	m_fNextUpdateTime = gpGlobals->curtime + 1.0f; 
}

//-----------------------------------------------------------------------------
// Purpose: Updates information about teams
//-----------------------------------------------------------------------------
void CTFCSScoreboard::UpdateTeamInfo()
{
	// update the team sections in the scoreboard
	int startTeam = TEAM_UNASSIGNED;
	startTeam = TEAM_BLUE;

	for ( int teamIndex = startTeam; teamIndex <= TEAM_RED; teamIndex++ )
	{
		// Make sure spectator is always skipped here.
		if ( teamIndex == TEAM_SPECTATOR )
			continue;

		wchar_t *teamName = NULL;
		C_Team *team = GetGlobalTeam( teamIndex );
		if ( team )
		{
			// choose dialog variables to set depending on team
			const char *pDialogVarTeamScore = NULL;
			const char *pDialogVarTeamPlayerCount = NULL;
			const char *pDialogVarTeamPing = NULL;
			switch ( teamIndex ) 
			{
				case TEAM_RED:
					teamName = g_pVGuiLocalize->Find( "#SDK_ScoreBoard_Red" );
					pDialogVarTeamScore = "red_teamscore";
					pDialogVarTeamPlayerCount = "red_teamplayercount";
					pDialogVarTeamPing = "red_teamping";
					break;
				case TEAM_BLUE:
					teamName = g_pVGuiLocalize->Find( "#SDK_ScoreBoard_Blue" );
					pDialogVarTeamScore = "blue_teamscore";
					pDialogVarTeamPlayerCount = "blue_teamplayercount";
					pDialogVarTeamPing = "blue_teamping";
					break;
				case TEAM_UNASSIGNED:
					teamName = g_pVGuiLocalize->Find( "#SDK_ScoreBoard_DM" );
					pDialogVarTeamPlayerCount = "dm_playercount";
					pDialogVarTeamPing = "dm_ping";
					break;
				default:
					Assert( false );
					break;
			}

			// update # of players on each team
			wchar_t name[64];
			wchar_t string1[1024];
			wchar_t wNumPlayers[6];
			_snwprintf( wNumPlayers, ARRAYSIZE( wNumPlayers ), L"%i", team->Get_Number_Players() );
			if ( !teamName && team )
			{
				g_pVGuiLocalize->ConvertANSIToUnicode( team->Get_Name(), name, sizeof( name ) );
				teamName = name;
			}

			if ( team->Get_Number_Players() == 1 )
				g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find( "#ScoreBoard_Player" ), 2, teamName, wNumPlayers );
			else
				g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find( "#ScoreBoard_Players" ), 2, teamName, wNumPlayers );

			// set # of players for team in dialog
			SetDialogVariable( pDialogVarTeamPlayerCount, string1 );

			// set team score in dialog
			if ( teamIndex != TEAM_UNASSIGNED )	// Don't accumulate deathmatch scores.
				SetDialogVariable( pDialogVarTeamScore, team->Get_Score() );			

			int pingsum = 0;
			int numcounted = 0;
			for( int playerIndex = 1 ; playerIndex <= MAX_PLAYERS; playerIndex++ )
			{
				if( g_PR->IsConnected( playerIndex ) && g_PR->GetTeam( playerIndex ) == teamIndex )
				{
					int ping = g_PR->GetPing( playerIndex );

					if ( ping >= 1 )
					{
						pingsum += ping;
						numcounted++;
					}
				}
			}

			if ( numcounted > 0 )
			{
				int ping = (int)( (float)pingsum / (float)numcounted );
				SetDialogVariable( pDialogVarTeamPing, ping );		
			}
			else
			{
				SetDialogVariable( pDialogVarTeamPing, "" );	
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the player list
//-----------------------------------------------------------------------------
void CTFCSScoreboard::UpdatePlayerList()
{
	m_pPlayerListRed->RemoveAll();
	m_pPlayerListBlue->RemoveAll();

	C_TFCSPlayer *pLocalPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( !pLocalPlayer )
		return;

	for( int playerIndex = 1 ; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if( g_PR->IsConnected( playerIndex ) )
		{
			SectionedListPanel *pPlayerList = NULL;
			
			switch ( g_PR->GetTeam( playerIndex ) )
			{
			case TEAM_RED:
				pPlayerList = m_pPlayerListRed;
				break;
			case TEAM_BLUE:
				pPlayerList = m_pPlayerListBlue;
				break;
			}

			if ( pPlayerList == NULL )
				continue;			

			KeyValues *pKeyValues = new KeyValues( "data" );
			GetPlayerScoreInfo( playerIndex, pKeyValues );

			int itemID = pPlayerList->AddItem( 0, pKeyValues );
			Color clr = g_PR->GetTeamColor( g_PR->GetTeam( playerIndex ) );
			pPlayerList->SetItemFgColor( itemID, clr );

			pKeyValues->deleteThis();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the spectator list
//-----------------------------------------------------------------------------
void CTFCSScoreboard::UpdateSpectatorList()
{
	C_TFCSPlayer *pLocalPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( !pLocalPlayer )
		return;

	char szSpectatorList[512] = "" ;
	int nSpectators = 0;
	for( int playerIndex = 1 ; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if ( ShouldShowAsSpectator( playerIndex ) )
		{
			if ( nSpectators > 0 )
			{
				Q_strncat( szSpectatorList, ", ", ARRAYSIZE( szSpectatorList ) );
			}

			Q_strncat( szSpectatorList, g_PR->GetPlayerName( playerIndex ), ARRAYSIZE( szSpectatorList ) );
			nSpectators++;
		}
	}

	wchar_t wzSpectators[512] = L"";
	if ( nSpectators > 0 )
	{
		const char *pchFormat = ( 1 == nSpectators ? "#ScoreBoard_Spectator" : "#ScoreBoard_Spectators" );

		wchar_t wzSpectatorCount[16];
		wchar_t wzSpectatorList[1024];
		_snwprintf( wzSpectatorCount, ARRAYSIZE( wzSpectatorCount ), L"%i", nSpectators );
		g_pVGuiLocalize->ConvertANSIToUnicode( szSpectatorList, wzSpectatorList, sizeof( wzSpectatorList ) );
		g_pVGuiLocalize->ConstructString( wzSpectators, sizeof(wzSpectators), g_pVGuiLocalize->Find( pchFormat), 2, wzSpectatorCount, wzSpectatorList );
	}

	SetDialogVariable( "spectators", wzSpectators );
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether the specified player index is a spectator
//-----------------------------------------------------------------------------
bool CTFCSScoreboard::ShouldShowAsSpectator( int iPlayerIndex )
{
	// see if player is connected
	if ( g_PR->IsConnected( iPlayerIndex ) ) 
	{
		// spectators show in spectator list
		int iTeam = g_PR->GetTeam( iPlayerIndex );

		if ( TEAM_SPECTATOR == iTeam )
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFCSScoreboard::FireGameEvent( IGameEvent *event )
{
	const char *type = event->GetName();

	if ( 0 == Q_strcmp( type, "server_spawn" ) )
	{		
		// set server name in scoreboard
		const char *hostname = event->GetString( "hostname" );
		wchar_t wzHostName[256];
		wchar_t wzServerLabel[256];
		g_pVGuiLocalize->ConvertANSIToUnicode( hostname, wzHostName, sizeof( wzHostName ) );
		g_pVGuiLocalize->ConstructString( wzServerLabel, sizeof(wzServerLabel), g_pVGuiLocalize->Find( "#Scoreboard_Server" ), 1, wzHostName );
		SetDialogVariable( "server", wzServerLabel );
	}

	if( IsVisible() )
	{
		Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new row to the scoreboard, from the playerinfo structure
//-----------------------------------------------------------------------------
bool CTFCSScoreboard::GetPlayerScoreInfo( int playerIndex, KeyValues *kv )
{
	//C_TFCSPlayerResource *tfcs_PR = dynamic_cast<C_TFCSPlayerResource *>( g_PR );
	//if ( !tfcs_PR )
	//	return true;

	// Clean up the player name
	const char *oldName = g_PR->GetPlayerName( playerIndex );
	int bufsize = strlen( oldName ) * 2 + 1;
	char *newName = (char *)_alloca( bufsize );
	UTIL_MakeSafeName( oldName, newName, bufsize );
	kv->SetString( "name", newName );

	kv->SetInt( "playerIndex", playerIndex );
	kv->SetInt( "frags", g_PR->GetPlayerScore( playerIndex ) );
	kv->SetInt( "deaths", g_PR->GetDeaths( playerIndex ) );
	kv->SetString( "class", "" );

	UpdatePlayerAvatar( playerIndex, kv );

	if ( g_PR->GetPing( playerIndex ) < 1 )
	{
		if ( g_PR->IsFakePlayer( playerIndex ) )
		{
			kv->SetString( "ping", "BOT" );
		}
		else
		{
			kv->SetString( "ping", "" );
		}
	}
	else
	{
		kv->SetInt( "ping", g_PR->GetPing( playerIndex ) );
	}

	C_TFCSPlayer *pLocalPlayer = C_TFCSPlayer::GetLocalTFCSPlayer();
	if ( !pLocalPlayer )
		return true;

	int team = g_PR->GetTeam( playerIndex );
	int localteam = pLocalPlayer->GetTeamNumber();

	// If we are on a team that shows class/status, and the local player is allowed to see this information
	if( ( localteam == TEAM_SPECTATOR || localteam == TEAM_UNASSIGNED || team == localteam ) )
	{
		// class name
		if( g_PR->IsConnected( playerIndex ) )
		{
			/*C_SDKTeam *pTeam = dynamic_cast<C_SDKTeam *>( GetGlobalTeam( team ) );

			Assert( pTeam );

			int cls = tfcs_PR->GetPlayerClass( playerIndex );

			char szClassName[64];
			szClassName[0] = '\0';

			if( cls != PLAYERCLASS_UNDEFINED )
			{
				const CSDKPlayerClassInfo &info = pTeam->GetPlayerClassInfo( cls );

				g_pVGuiLocalize->ConvertUnicodeToANSI( g_pVGuiLocalize->Find( info.m_szPrintName ), szClassName, sizeof(szClassName) );
			}

			kv->SetString( "class", szClassName );*/
		}
		else
		{
			Assert(0);
		}

		// status
		// display whether player is alive or dead (all players see this for all other players on both teams)
		kv->SetInt( "status", g_PR->IsAlive( playerIndex ) ?  0 : m_iImageDead );
	}

	if ( g_PR->IsHLTV( playerIndex ) )
	{
		// show #spectators in class field, it's transmitted as player's score
		char numspecs[32];
		Q_snprintf( numspecs, sizeof( numspecs ), "%i Spectators", m_HLTVSpectators );
		kv->SetString( "class", numspecs );
	}

	return true;
}

void CTFCSScoreboard::UpdateItemVisibiity()
{
	// Red Labels _ON_
	m_pPlayerListRed->SetVisible( true );
	m_pPlayerCountLabel_Red->SetVisible( true );
	m_pScoreHeader_Red->SetVisible( true );
	m_pScoreLabel_Red->SetVisible( true );
	m_pDeathsHeader_Red->SetVisible( true );
	m_pPingHeader_Red->SetVisible( true );
	m_pPingLabel_Red->SetVisible( true );

	// Blue Labels _ON_
	m_pPlayerListBlue->SetVisible( true );
	m_pPlayerCountLabel_Blue->SetVisible( true );
	m_pScoreHeader_Blue->SetVisible( true );
	m_pScoreLabel_Blue->SetVisible( true );
	m_pDeathsHeader_Blue->SetVisible( true );
	m_pPingHeader_Blue->SetVisible( true );
	m_pPingLabel_Blue->SetVisible( true );

	// Vertical Line _ON_
	m_pVertLine->SetVisible( true );

	// Restore the size to the original incase we've switched from DM -> Teams and back.
	SetSize(m_iStoredScoreboardWidth, GetTall() );
}
