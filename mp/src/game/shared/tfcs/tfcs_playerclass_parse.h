#ifndef TFCS_PLAYERCLASS_PARSE_H
#define TFCS_PLAYERCLASS_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "playerclass_info_parse.h"
#include "networkvar.h"

struct TFCSPlayerClassInfo_t
{
	char m_szClassName[ MAX_PLAYERCLASS_NAME_LENGTH ];
	char m_szArmsModel[ MAX_PLAYERCLASS_NAME_LENGTH];
	char m_szModelName[ MAX_PLAYERCLASS_NAME_LENGTH ];
	char m_szLocalizableName[ MAX_PLAYERCLASS_NAME_LENGTH ];

	float m_flMaxSpeed;
	int m_iMaxArmor;
	int m_iMaxHealth;
	int m_iSpawnArmor;
	float m_flArmorClass;
	int m_aMaxAmmo[ AMMO_LAST ];
	int m_aSpawnAmmo[AMMO_LAST ];

	//Weapons
	int m_aWeapons[ TFCS_MAX_WEAPON_SLOTS ];

	// Grenades
	int m_iGrenType1;
	int m_iGrenType2;

	bool m_bParsed;

	TFCSPlayerClassInfo_t();
	void Parse( const char *pszClassName );

private:
	void ParseData( KeyValues *pKeyValuesData );
};

void InitPlayerClasses( void );
TFCSPlayerClassInfo_t *GetClassData( int iClass );

class CTFCSPlayerClass
{
public:
	DECLARE_EMBEDDED_NETWORKVAR()
	DECLARE_CLASS_NOBASE( CTFCSPlayerClass );

	CTFCSPlayerClass();

	int GetClassIndex( void ) { return m_iClass; }
	bool Init( int iClass );

	const char *GetName( void ) const		{ return GetClassData( m_iClass )->m_szClassName; }
	const char *GetModelName( void ) const	{ return GetClassData( m_iClass )->m_szModelName; }
	float GetMaxSpeed( void )				{ return GetClassData( m_iClass )->m_flMaxSpeed; }
	int GetMaxHealth( void )				{ return GetClassData( m_iClass )->m_iMaxHealth; }
	int GetMaxArmor( void )					{ return GetClassData( m_iClass )->m_iMaxArmor; }
	
	TFCSPlayerClassInfo_t *GetData( int iClass );

	CNetworkVar( int, m_iClass );
};
#endif // TFCS_PLAYERCLASS_PARSE_H