//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CREATEMULTIPLAYERGAMESERVERPAGE_H
#define CREATEMULTIPLAYERGAMESERVERPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/cvartogglecheckbutton.h>

//-----------------------------------------------------------------------------
// Purpose: server options page of the create game server dialog
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameServerPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE( CCreateMultiplayerGameServerPage, vgui::PropertyPage );

public:
	CCreateMultiplayerGameServerPage(vgui::Panel *parent, const char *name);
	~CCreateMultiplayerGameServerPage();

	// returns currently entered information about the server
	void SetMap(const char *name);
	bool IsRandomMapSelected();
	const char *GetMapName();

protected:
	virtual void OnApplyChanges();

private:
	void LoadMapList();
	void LoadMaps( const char *pszPathID );

	vgui::ComboBox *m_pMapList;

	enum { DATA_STR_LENGTH = 64 };
	char m_szMapName[DATA_STR_LENGTH];
};


#endif // CREATEMULTIPLAYERGAMESERVERPAGE_H
