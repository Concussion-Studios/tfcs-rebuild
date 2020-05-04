//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "EventLog.h"
#include "KeyValues.h"

#include "tfc_player.h"
#include "tfc_team.h"

class CTFCEventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual ~CTFCEventLog() {};

public:
	bool PrintEvent( IGameEvent * event )	// override virtual function
	{
		if ( !PrintTFCEvent( event ) )
		{
			return BaseClass::PrintEvent( event );
		}
		else
		{
			return true;
		}

		return false;
	}
	bool Init()
	{
		BaseClass::Init();

		ListenForGameEvent( "player_death" );
		ListenForGameEvent( "player_hurt" );
		ListenForGameEvent( "player_changeclass" );

		return true;
	}
protected:

	bool PrintTFCEvent( IGameEvent * event )	// print Mod specific logs
	{
		const char *eventName = event->GetName();
	
		if ( !Q_strncmp( eventName, "server_", strlen("server_")) )
		{
			return false; // ignore server_ messages, always.
		}
		if ( FStrEq( eventName, "player_changeclass" ) )
		{
			const int userid = event->GetInt( "userid" );
			CBasePlayer *pPlayer = UTIL_PlayerByUserId( userid );

			if ( !pPlayer )
			{
				return false;
			}

			int iClass = event->GetInt("class");
			int iTeam = pPlayer->GetTeamNumber();

			if ( iTeam != TFC_TEAM_BLUE && iTeam != TFC_TEAM_RED /*|| iTeam != TEAM_UNASSIGNED*/ )
				return true;

			CTFCTeam *pTeam = GetGlobalTFCTeam( iTeam );

			if ( iClass == PLAYERCLASS_RANDOM )
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" changed role to \"Random\"\n",  
					pPlayer->GetPlayerName(),
					userid,
					pPlayer->GetNetworkIDString(),
					pTeam->GetName()
					);
			}
			else if ( iClass < GetGlobalTFCTeam(iTeam)->GetNumPlayerClasses() )
			{
				const CTFCPlayerClassInfo &pInfo = GetGlobalTFCTeam(iTeam)->GetPlayerClassInfo( iClass );

				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" changed role to \"%s\"\n",  
					pPlayer->GetPlayerName(),
					userid,
					pPlayer->GetNetworkIDString(),
					pTeam->GetName(),
					pInfo.m_szPrintName
					);
			}
			return true;
		}

		return false;
	}

};

CTFCEventLog g_TFCEventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &g_TFCEventLog;
}

