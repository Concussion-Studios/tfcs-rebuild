//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TFCS_VIEWPORT_H
#define TFCS_VIEWPORT_H
#ifdef _WIN32
#pragma once
#endif

#include "baseviewport.h"


using namespace vgui;

namespace vgui 
{
	class Panel;
}

class TFCSViewport : public CBaseViewport
{

private:
	DECLARE_CLASS_SIMPLE( TFCSViewport, CBaseViewport );

public:

	IViewPortPanel* CreatePanelByName(const char *szPanelName);
	void CreateDefaultPanels( void );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
		
	int GetDeathMessageStartHeight( void );
};


#endif // TFCSViewport_H
