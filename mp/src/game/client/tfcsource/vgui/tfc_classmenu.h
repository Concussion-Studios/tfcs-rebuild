//======== Copyright © 1996-2008, Valve Corporation, All rights reserved. =========//
//
// Purpose: 
//
// $NoKeywords: $
//=================================================================================//

#ifndef TFC_CLASSMENU_H
#define TFC_CLASSMENU_H

#include <classmenu.h>
#include <vgui_controls/EditablePanel.h>
#include <FileSystem.h>
#include "iconpanel.h"
#include "mouseoverpanelbutton.h"
#include <vgui_controls/CheckButton.h>

class CTFCClassInfoPanel : public vgui::EditablePanel
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassInfoPanel, vgui::EditablePanel );

public:
	CTFCClassInfoPanel( vgui::Panel *parent, const char *panelName ) : vgui::EditablePanel( parent, panelName )	{}
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual vgui::Panel *CreateControlByName( const char *controlName );

private:
	vgui::HFont m_hFont;
};

class CTFCClassMenu : public CClassMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassMenu, CClassMenu );

public:
	CTFCClassMenu( IViewPort *pViewPort );
	CTFCClassMenu( IViewPort *pViewPort, const char *panelName );
	virtual ~CTFCClassMenu();

	virtual void Update( void );
	virtual Panel *CreateControlByName( const char *controlName );
	virtual void OnTick( void );
	virtual void OnKeyCodePressed(KeyCode code);
	virtual void SetVisible( bool state );

	// helper functions
	void SetVisibleButton( const char *textEntryName, bool state );
	virtual void ShowPanel( bool bShow );

	MESSAGE_FUNC_CHARPTR( OnShowPage, "ShowPage", page );

	virtual int GetTeamNumber( void ) = 0;

	// Background panel -------------------------------------------------------

public:
	virtual void PaintBackground( void ) { /* Draw nothing */ }
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	bool m_backgroundLayoutFinished;

	// End background panel ---------------------------------------------------

private:
	CTFCClassInfoPanel *m_pClassInfoPanel;
	MouseOverButton<CTFCClassInfoPanel> *m_pInitialButton;

	int m_iActivePlayerClass;
	int m_iLastPlayerClassCount;
	int	m_iLastClassLimit;

	ButtonCode_t m_iClassMenuKey;
};

class CTFCClassMenu_Blue : public CTFCClassMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassMenu_Blue, CTFCClassMenu );

public:
	CTFCClassMenu_Blue::CTFCClassMenu_Blue(IViewPort *pViewPort) : BaseClass(pViewPort, PANEL_CLASS_BLUE) {	LoadControlSettings( "Resource/UI/ClassMenu_Blue.res" ); }
	virtual const char *GetName( void )	{ return PANEL_CLASS_BLUE; }
	virtual int GetTeamNumber( void ) {	return TEAM_BLUE; }
};

class CTFCClassMenu_Red : public CTFCClassMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassMenu_Red, CTFCClassMenu );

public:
	CTFCClassMenu_Red::CTFCClassMenu_Red(IViewPort *pViewPort) : BaseClass(pViewPort, PANEL_CLASS_RED)	{ LoadControlSettings( "Resource/UI/ClassMenu_Red.res" ); }
	virtual const char *GetName( void )	{ return PANEL_CLASS_RED; }
	virtual int GetTeamNumber( void ) {	return TEAM_RED; }
};

/*class CTFCClassMenu_Green : public CTFCClassMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassMenu_Green, CTFCClassMenu );

public:
	CTFCClassMenu_Green::CTFCClassMenu_Green(IViewPort *pViewPort) : BaseClass(pViewPort, PANEL_CLASS_GREEN) {	LoadControlSettings( "Resource/UI/ClassMenu_Green.res" ); }
	virtual const char *GetName( void )	{ return PANEL_CLASS_GREEN; }
	virtual int GetTeamNumber( void ) {	return TEAM_GREEN; }
};

class CTFCClassMenu_Yellow : public CTFCClassMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCClassMenu_Yellow, CTFCClassMenu );

public:
	CTFCClassMenu_Yellow::CTFCClassMenu_Yellow(IViewPort *pViewPort) : BaseClass(pViewPort, PANEL_CLASS_YELLOW)	{ LoadControlSettings( "Resource/UI/ClassMenu_Yellow.res" ); }
	virtual const char *GetName( void )	{ return PANEL_CLASS_YELLOW; }
	virtual int GetTeamNumber( void ) {	return TEAM_YELLOW; }
};*/

#endif //TFC_CLASSMENU_H