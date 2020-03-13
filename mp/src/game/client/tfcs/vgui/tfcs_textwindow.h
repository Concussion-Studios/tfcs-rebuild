//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TFCS_TEXTWINDOW_H
#define TFCS_TEXTWINDOW_H
#ifdef _WIN32
#pragma once
#endif

#include <vguitextwindow.h>

//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CTFCSTextWindow : public CTextWindow
{
private:
	DECLARE_CLASS_SIMPLE(CTFCSTextWindow, CTextWindow);
	
public:
	CTFCSTextWindow( IViewPort *pViewPort );

	virtual void Update();
	virtual void SetVisible( bool state );

	// Background panel -------------------------------------------------------

public:
	virtual void PaintBackground() {}
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	bool m_backgroundLayoutFinished;

	// End background panel ---------------------------------------------------
};


#endif // TFCS_TEXTWINDOW_H
