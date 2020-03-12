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

#define SCREEN_FILE		"scripts/vgui_screens.txt"

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class ClientModeTFCS : public ClientModeShared
{
	DECLARE_CLASS( ClientModeTFCS, ClientModeShared );
public:

	ClientModeTFCS();
	~ClientModeTFCS();

	virtual void	Init();

};

//-----------------------------------------------------------------------------
// Purpose: this is the viewport that contains all the hud elements
//-----------------------------------------------------------------------------
class CHudViewport : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE( CHudViewport, CBaseViewport );

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void CreateDefaultPanels( void ) { /* don't create any panels yet*/ };
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

#endif // CLIENTMODE_TFCS_H
