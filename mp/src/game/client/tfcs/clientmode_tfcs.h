#ifndef CLIENTMODE_TFCS_H
#define CLIENTMODE_TFCS_H
#ifdef _WIN32
#pragma once
#endif

#include "ivmodemanager.h"
#include "panelmetaclassmgr.h"
#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>
#include "tfcs_viewport.h"

#define SCREEN_FILE		"scripts/vgui_screens.txt"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class ClientModeTFCS : public ClientModeShared
{
	DECLARE_CLASS( ClientModeTFCS, ClientModeShared );

public:

					ClientModeTFCS();
	virtual			~ClientModeTFCS();

	virtual void	Init();
	virtual void	InitViewport();
	virtual void	FireGameEvent( IGameEvent *event );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual bool	DoPostScreenSpaceEffects( const CViewSetup *pSetup );
	virtual float	GetViewModelFOV( void );
	virtual int		GetDeathMessageStartHeight( void );
	virtual void	PostRenderVGui() {}
	virtual bool	CanRecordDemo( char *errorMsg, int length ) const;
	virtual int		KeyInput( int down, ButtonCode_t keynum, const char* pszCurrentBinding );
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CTFCSModeManager : public IVModeManager
{
public:
				CTFCSModeManager( void ) {}
	virtual		~CTFCSModeManager( void ) {}

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force ) {}
	virtual void	OverrideView( CViewSetup *pSetup ) {}
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd ) {}
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

extern IClientMode *GetClientModeNormal();
extern ClientModeTFCS* GetClientModeTFCS();

#endif // CLIENTMODE_TFCS_H
