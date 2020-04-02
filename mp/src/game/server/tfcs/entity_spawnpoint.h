//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Team Spawning Entity.
//
//=============================================================================//
#ifndef ENTITY_SPAWNPOINT_H
#define ENTITY_SPAWNPOINT_H
#ifdef _WIN32
#pragma once
#endif

class CTeamControlPoint;
class CTeamControlPointRound;

DECLARE_AUTO_LIST( ITFCSSpawnPointAutoList )

//=============================================================================//
// Purpose:
//=============================================================================//
class CTFCSSpawnPoint : public CPointEntity, public ITFCSSpawnPointAutoList
{
public:
	DECLARE_CLASS( CTFCSSpawnPoint, CPointEntity );

	CTFCSSpawnPoint();

	virtual void Spawn( void );
	virtual void Activate( void );

	bool IsDisabled( void ) { return m_bDisabled; }
	void SetDisabled( bool bDisabled ) { m_bDisabled = bDisabled; }

	// Inputs/Outputs.
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputRoundSpawn( inputdata_t &inputdata );

	int DrawDebugTextOverlays(void);

	CHandle<CTeamControlPoint> GetControlPoint( void ) { return m_hControlPoint; }
	CHandle<CTeamControlPointRound> GetRoundBlueSpawn( void ) { return m_hRoundBlueSpawn; }
	CHandle<CTeamControlPointRound> GetRoundRedSpawn( void ) { return m_hRoundRedSpawn; }

private:
	bool	m_bDisabled;		// Enabled/Disabled?

	string_t m_iszControlPointName;
	string_t m_iszRoundBlueSpawn;
	string_t m_iszRoundRedSpawn;

	CHandle<CTeamControlPoint>		m_hControlPoint;
	CHandle<CTeamControlPointRound>	m_hRoundBlueSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundRedSpawn;

	DECLARE_DATADESC();
};

#endif // ENTITY_TFSTART_H