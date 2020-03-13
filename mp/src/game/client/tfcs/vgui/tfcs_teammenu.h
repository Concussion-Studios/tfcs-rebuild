//======== Copyright © 1996-2008, Valve Corporation, All rights reserved. =========//
//
// Purpose: 
//
// $NoKeywords: $
//=================================================================================//
#ifndef TFCS_TEAMMENU_H
#define TFCS_TEAMMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "teammenu.h"

class CTFCSTeamMenu : public CTeamMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFCSTeamMenu, CTeamMenu );

	CTFCSTeamMenu( IViewPort *pViewPort );
	virtual ~CTFCSTeamMenu();

	void Update();
	virtual void SetVisible( bool state );

public:

	// VGUI2 override
	void OnCommand( const char* command );

	// helper functions
	void SetVisibleButton( const char* textEntryName, bool state );

	// Background panel -------------------------------------------------------

public:
	virtual void PaintBackground( void ) { /* Draw nothing */ }
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	bool m_backgroundLayoutFinished;

	// End background panel ---------------------------------------------------
};

#endif //TFCS_CLASSMENU_H