//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef C_BASEHLPLAYER_H
#define C_BASEHLPLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseplayer.h"

class C_BaseHLPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_BaseHLPlayer, C_BasePlayer );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void OnDataChanged( DataUpdateType_t type );

	C_BaseHLPlayer();

	virtual bool ShouldRegenerateOriginFromCellBits() const	{ return true; }
	virtual	void BuildTransformations( CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed );

	// Input handling
	virtual bool CreateMove( float flInputSampleTime, CUserCmd *pCmd );
	void PerformClientSideObstacleAvoidance( float flFrameTime, CUserCmd *pCmd );
	void PerformClientSideNPCSpeedModifiers( float flFrameTime, CUserCmd *pCmd );

public:

	// Do we have an object?
	bool PlayerHasObject() { return m_bPlayerPickedUpObject; }
	bool m_bPlayerPickedUpObject;

	EHANDLE m_hClosestNPC;
	float m_flSpeedModTime;
	float m_flExitSpeedMod;

private:
	bool TestMove( const Vector &pos, float fVertDist, float radius, const Vector &objPos, const Vector &objDir );

	float m_flSpeedMod;
};
#endif