#include "cbase.h"
#include "c_tfcs_player.h"
//#include "multiplayer_animstate.h"
#include "view.h"
#include "props_shared.h"
#include "c_fire_smoke.h"
#include "fx.h"
#include "prediction.h"
#include "view_scene.h" // for tone mapping reset
#include "input.h"
#include "collisionutils.h"
#include "c_team.h"
#include "obstacle_pushaway.h"

#include "tier0/memdbgon.h"

#ifdef CTFCSPlayer
	#undef CTFCSPlayer
#endif

#define CYCLELATCH_TOLERANCE 0.15f

void FX_BloodSpray( const Vector& origin, const Vector& normal, float scale, unsigned char r, unsigned char g, unsigned char b, int flags );
C_EntityFlame *FireEffect( C_BaseAnimating *pTarget, C_BaseEntity *pServerFire, float *flScaleEnd, float *flTimeStart, float *flTimeEnd );

extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

ConVar cl_max_separation_force ( "cl_max_separation_force", "256", FCVAR_CHEAT | FCVAR_HIDDEN );

ConVar cl_playergib_forceup( "cl_playergib_forceup", "1.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "Upward added velocity for gibs." );
ConVar cl_playergib_force( "cl_playergib_force", "500.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "Gibs force." );
ConVar cl_playergib_maxspeed( "cl_playergib_maxspeed", "400", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "Max gib speed." );

ConVar cl_ragdoll_physics_enable( "cl_ragdoll_physics_enable", "1", 0, "Enable/disable ragdoll physics." );
ConVar cl_ragdoll_fade_time( "cl_ragdoll_fade_time", "15", FCVAR_CLIENTDLL );
ConVar cl_ragdoll_forcefade( "cl_ragdoll_forcefade", "0", FCVAR_CLIENTDLL );
ConVar cl_ragdoll_pronecheck_distance( "cl_ragdoll_pronecheck_distance", "64", FCVAR_GAMEDLL );

// ***************** C_TEPlayerAnimEvent **********************

void C_TEPlayerAnimEvent::PostDataUpdate( DataUpdateType_t updateType )
{
	// Create the effect.
	auto* pPlayer = ToTFCSPlayer( m_hPlayer.Get() );
	if ( pPlayer && !pPlayer->IsDormant() )
		pPlayer->DoAnimationEvent( ( PlayerAnimEvent_t )m_iEvent.Get(), m_nData );
}

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_iEvent ) ),
	RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

// ***************** C_TFCSRagdoll **********************

IMPLEMENT_CLIENTCLASS_DT( C_TFCSRagdoll, DT_TFCSRagdoll, CTFCSRagdoll )
	RecvPropVector( RECVINFO( m_vecRagdollOrigin ) ),
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_nModelIndex ) ),
	RecvPropInt( RECVINFO( m_nForceBone ) ),
	RecvPropVector( RECVINFO( m_vecForce ) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) ),
	RecvPropBool( RECVINFO( m_bGib ) ),
	RecvPropBool( RECVINFO( m_bBurning ) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( tfcs_ragdoll, C_TFCSRagdoll );

C_TFCSRagdoll::~C_TFCSRagdoll()
{
	PhysCleanupFrictionSounds( this );
}

void C_TFCSRagdoll::Interp_Copy( C_BaseAnimatingOverlay* pSourceEntity )
{
	if ( !pSourceEntity )
		return;

	VarMapping_t* pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t* pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t* pDestEntry = &pDest->m_Entries[i];
		const char* pszName = pDestEntry->watcher->GetDebugName();
		for ( int j = 0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t* pSrcEntry = &pSrc->m_Entries[j];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pszName ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

ShadowType_t C_TFCSRagdoll::ShadowCastType( void )
{
	return BaseClass::ShadowCastType();
}

void C_TFCSRagdoll::ImpactTrace( trace_t* pTrace, int iDamageType, const char* pCustomImpactName )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if ( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght
				
		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;  

		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		// Blood spray!
		FX_BloodSpray( hitpos, dir, 3, 72, 0, 0, FX_BLOODSPRAY_ALL  );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );	
		//Tony; throw in some bleeds! - just use a generic value for damage.
		TraceBleed( 40, dir, pTrace, iDamageType );
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_TFCSRagdoll::IsRagdollVisible()
{
	Vector vMins = Vector( -1, -1, -1 );	//WorldAlignMins();
	Vector vMaxs = Vector( 1, 1, 1 );		//WorldAlignMaxs();

	Vector origin = GetAbsOrigin();

	if ( !engine->IsBoxInViewCluster( vMins + origin, vMaxs + origin ) )
		return false;
	else if ( engine->CullBox( vMins + origin, vMaxs + origin ) )
		return false;

	return true;
}

void C_TFCSRagdoll::ClientThink( void )
{
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	if ( m_bFadingOut == true )
	{
		int iAlpha = GetRenderColor().a;
		int iFadeSpeed = 600.0f;

		iAlpha = max( iAlpha - ( iFadeSpeed * gpGlobals->frametime ), .0f );

		SetRenderMode( kRenderTransAlpha );
		SetRenderColorA( iAlpha );

		if ( iAlpha == 0 )
			EndFadeOut(); // remove clientside ragdoll

		return;
	}

	// if the player is looking at us, delay the fade
	if ( IsRagdollVisible() )
	{
		if ( cl_ragdoll_forcefade.GetBool() )
		{
			m_bFadingOut = true;
			float flDelay = cl_ragdoll_fade_time.GetFloat() * 0.33f;
			m_fDeathTime = gpGlobals->curtime + flDelay;

			// If we were just fully healed, remove all decals
			RemoveAllDecals();
		}

		StartFadeOut( cl_ragdoll_fade_time.GetFloat() * 0.33f );
		return;
	}

	if ( m_fDeathTime > gpGlobals->curtime )
		return;

	EndFadeOut(); // remove clientside ragdoll
}

void C_TFCSRagdoll::StartFadeOut( float fDelay )
{
	if ( !cl_ragdoll_forcefade.GetBool() )
		m_fDeathTime = gpGlobals->curtime + fDelay;

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}


void C_TFCSRagdoll::EndFadeOut()
{
	SetNextClientThink( CLIENT_THINK_NEVER );
	ClearRagdoll();
	SetRenderMode( kRenderNone );
	UpdateVisibility();
}

void C_TFCSRagdoll::CreateTFCSRagdoll( void )
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_BasePlayer* pPlayer = dynamic_cast<C_BasePlayer*>( m_hPlayer.Get() );

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		// move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t* varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = ( pPlayer != C_BasePlayer::GetLocalPlayer() );
		if ( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );

			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = pPlayer->GetSequence();
			if ( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}

			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			Interp_Reset( varMap );
		}

		m_nBody = pPlayer->GetBody();
		m_nSkin = pPlayer->GetSkin();
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );

	}

	SetModelIndex( m_nModelIndex );

	// Make us a ragdoll..
	m_nRenderFX = kRenderFxRagdoll;

	matrix3x4_t boneDelta0[MAXSTUDIOBONES];
	matrix3x4_t boneDelta1[MAXSTUDIOBONES];
	matrix3x4_t currentBones[MAXSTUDIOBONES];
	const float boneDt = 0.05f;

	if ( pPlayer && !pPlayer->IsDormant() )
		pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	else
		GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );

	InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );

	if ( m_bBurning )
		FireEffect( this, nullptr, nullptr, nullptr, nullptr );

	// Fade out the ragdoll in a while
	StartFadeOut( cl_ragdoll_fade_time.GetFloat() );
	SetNextClientThink( gpGlobals->curtime + cl_ragdoll_fade_time.GetFloat() * 0.33f );
}

void C_TFCSRagdoll::CreateTFCSGibs( void )
{
	bool bCreatedGibs = false;

	C_TFCSPlayer *pPlayer = NULL;
	if ( m_hPlayer )
		pPlayer = dynamic_cast<C_TFCSPlayer*>( m_hPlayer.Get() );

	if ( pPlayer && ( pPlayer->m_hFirstGib == NULL ) )
	{
		Vector vecVelocity = m_vecForce + m_vecRagdollVelocity;
		VectorNormalize(vecVelocity);
		bCreatedGibs = pPlayer->CreatePlayerGibs( m_vecRagdollOrigin, vecVelocity, m_vecForce.Length(), m_bBurning );
	}

	if ( bCreatedGibs )
		EndFadeOut();
	else
		CreateTFCSRagdoll();
}


void C_TFCSRagdoll::OnDataChanged( DataUpdateType_t type )
{
	bool modelchanged = false;

	// UNDONE: The base class does this as well.  So this is kind of ugly
	// but getting a model by index is pretty cheap...
	const model_t *pModel = modelinfo->GetModel( GetModelIndex() );

	if ( pModel != GetModel() )
		modelchanged = true;

	C_BaseEntity::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		bool bCreateRagdoll = true;

		// Get the player.
		EHANDLE hPlayer = m_hPlayer;
		if ( hPlayer )
		{
			// If we're getting the initial update for this player (e.g., after resetting entities after
			//  lots of packet loss, then don't create gibs, ragdolls if the player and it's gib/ragdoll
			//  both show up on same frame.
			if ( abs( hPlayer->GetCreationTick() - gpGlobals->tickcount ) < TIME_TO_TICKS( 1.0f ) )
				bCreateRagdoll = false;
		}
		else if ( C_BasePlayer::GetLocalPlayer() )
		{
			// Ditto for recreation of the local player
			if ( abs( C_BasePlayer::GetLocalPlayer()->GetCreationTick() - gpGlobals->tickcount ) < TIME_TO_TICKS( 1.0f ) )
				bCreateRagdoll = false;
		}

		if ( bCreateRagdoll )
		{
			if ( m_bGib )
				CreateTFCSGibs();
			else
				CreateTFCSRagdoll();
		}

		CStudioHdr* hdr = GetModelPtr();
		Assert( hdr );
		if ( !hdr )
			return;

		for ( int i = 0; i < hdr->GetNumPoseParameters(); i++ )
			SetPoseParameter( hdr, i, 0.0 );
	}
	else
	{
		// Make us a ragdoll..
		if ( m_pRagdoll )
			m_nRenderFX = kRenderFxRagdoll;
	}
}

int C_TFCSRagdoll::GetPlayerEntIndex() const
{
	if ( m_hPlayer )
		return m_hPlayer->entindex();

	return 0;
}

IRagdoll* C_TFCSRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

void C_TFCSRagdoll::UpdateOnRemove( void )
{
	VPhysicsSetObject( NULL );

	BaseClass::UpdateOnRemove();
}

C_BaseAnimating* C_TFCSPlayer::BecomeRagdollOnClient()
{
	return nullptr;
}

IRagdoll* C_TFCSPlayer::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		auto* pRagdoll = static_cast< C_TFCSRagdoll* >( m_hRagdoll.Get() );
		if ( !pRagdoll )
			return NULL;

		return pRagdoll->GetIRagdoll();
	}
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: clear out any face/eye values stored in the material system
//-----------------------------------------------------------------------------
void C_TFCSRagdoll::SetupWeights( const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights )
{
	// While we're dying, we want to mimic the facial animation of the player. Once they're dead, we just stay as we are.
	if ( ( m_hPlayer && m_hPlayer->IsAlive() ) || !m_hPlayer )
		BaseClass::SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );
	else if ( m_hPlayer )
		m_hPlayer->SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );

	CStudioHdr* hdr = GetModelPtr();
	if ( !hdr )
		return;

	/*int nFlexDescCount = hdr->numflexdesc();
	if ( nFlexDescCount )
	{
		Assert( !pFlexDelayedWeights );
		memset( pFlexWeights, 0, nFlexWeightCount * sizeof( float ) );
	}*/

	if ( m_iEyeAttachment > 0 )
	{
		matrix3x4_t attToWorld;
		if ( GetAttachment( m_iEyeAttachment, attToWorld ) )
		{
			Vector local, tmp;
			local.Init( 1000.0f, 0.0f, 0.0f );
			VectorTransform( local, attToWorld, tmp );
			modelrender->SetViewTarget( GetModelPtr(), GetBody(), tmp );
		}
	}
}

// ***************** C_TFCSPlayer **********************

// ------------------------------------------------------------------------------------------ //
// Data tables and prediction tables.
// ------------------------------------------------------------------------------------------ //
BEGIN_PREDICTION_DATA( C_TFCSPlayer )
END_PREDICTION_DATA()

BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSLocalPlayerExclusive )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ), // RECVINFO_NAME redirects the received var to m_vecNetworkOrigin for interpolation purposes
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),

	//RecvPropVector( RECVINFO() )
	RecvPropFloat( RECVINFO( m_ArmorClass ) ),
	RecvPropInt( RECVINFO( m_MaxArmor ) ),
	RecvPropFloat( RECVINFO( m_flConcussTime ) ),
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( C_TFCSPlayer, DT_TFCSNonLocalPlayerExclusive )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ), // RECVINFO_NAME again
	RecvPropFloat( RECVINFO( m_angEyeAngles[0]) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1]) ),
	RecvPropInt( RECVINFO( m_cycleLatch ), 0, &C_TFCSPlayer::RecvProxy_CycleLatch ),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_TFCSPlayer, DT_TFCSPlayer, CTFCSPlayer )
	RecvPropDataTable( RECVINFO_DT( m_Shared ), 0, &REFERENCE_RECV_TABLE( DT_TFCSPlayerShared ) ),

	RecvPropInt( RECVINFO( m_iRealSequence ) ),
	RecvPropBool( RECVINFO( m_bSpawnInterpCounter ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),

	RecvPropDataTable( "tfcs_localdata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSLocalPlayerExclusive ) ),
	RecvPropDataTable( "tfcs_nonlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_TFCSNonLocalPlayerExclusive ) ),
END_RECV_TABLE()

ITFCSPlayerAnimState* CreatePlayerAnimState( C_TFCSPlayer *pPlayer );

void C_TFCSPlayer::RecvProxy_CycleLatch( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	C_TFCSPlayer* pPlayer = static_cast< C_TFCSPlayer* >( pStruct );

	float flServerCycle = ( float )pData->m_Value.m_Int / 16.0f;
	float flCurCycle = pPlayer->GetCycle();

	// The cycle is way out of sync.
	if ( fabsf( flCurCycle - flServerCycle) > CYCLELATCH_TOLERANCE )
		pPlayer->SetServerIntendedCycle( flServerCycle );
}

C_TFCSPlayer::C_TFCSPlayer() : m_iv_angEyeAngles( "C_TFCSPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this );
	m_angEyeAngles.Init();

	m_Shared.Init( this );

	m_bSpawnInterpCounterCache = false;
	m_bSpawnInterpCounter = false;

	SetPredictionEligible( true );

	m_flServerCycle = -1.0f;
	m_cycleLatch = 0;

// cant interpolate ... buggy?  it keeps resetting the angle to 0,0,0
//	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	//m_fNextThinkPushAway = 0.0f;
}

C_TFCSPlayer::~C_TFCSPlayer()
{
	if ( GetAnimState() )
		GetAnimState()->Release();
}

C_TFCSPlayer* C_TFCSPlayer::GetLocalTFCSPlayer()
{
	return ToTFCSPlayer( C_BasePlayer::GetLocalPlayer() );
}

void C_TFCSPlayer::ClientThink()
{
	// Pass on through to the base class.
	BaseClass::ClientThink();

	UpdateIDTarget();

	// Avoidance
	if ( gpGlobals->curtime >= m_fNextThinkPushAway )
	{
		PerformObstaclePushaway( this );
		m_fNextThinkPushAway = gpGlobals->curtime + PUSHAWAY_THINK_INTERVAL;
	}
}

int C_TFCSPlayer::DrawModel( int flags )
{
	if ( !m_bReadyToDraw )
		return 0;

	return BaseClass::DrawModel( flags );
}

void C_TFCSPlayer::TeamChange( int iNewTeam )
{
	BaseClass::TeamChange( iNewTeam );

	// The team number hasn't been updated yet.
	int iOldTeam = GetTeamNumber();
	C_BaseEntity::ChangeTeam( iNewTeam );

	// Reset back to old team just in case something uses it.
	C_BaseEntity::ChangeTeam( iOldTeam );

	TeamChangeStatic( iNewTeam );
}

void C_TFCSPlayer::TeamChangeStatic( int iNewTeam )
{
	// It's possible to receive events from the server before our local player is created.
	// All crucial things that don't rely on local player
	// should be put here.
	const char *pTeamConfig = "exec team_red.cfg";

	switch ( iNewTeam )
	{
	case TEAM_RED:
		pTeamConfig = "exec team_red.cfg";
		break;
	case TEAM_BLUE:
		pTeamConfig = "exec team_blue.cfg";
		break;
	case TEAM_GREEN:
		pTeamConfig = "exec team_green.cfg";
		break;
	case TEAM_YELLOW:
		pTeamConfig = "exec team_yellow.cfg";
		break;
	}

	if ( !( iNewTeam == TEAM_UNASSIGNED || iNewTeam == TEAM_SPECTATOR ) )
		engine->ClientCmd_Unrestricted( pTeamConfig );

}

ShadowType_t C_TFCSPlayer::ShadowCastType( void )
{
	if ( !IsVisible()  || IsLocalPlayer() )
		return SHADOWS_NONE;

	if ( IsEffectActive( EF_NODRAW | EF_NOSHADOW ) )
		return SHADOWS_NONE;

	// If in ragdoll mode.
	if ( m_nRenderFX == kRenderFxRagdoll )
		return SHADOWS_NONE;

	C_BasePlayer *pLocalPlayer = GetLocalPlayer();

	// if we're first person spectating this player
	if ( pLocalPlayer &&
		 pLocalPlayer->GetObserverTarget() == this &&
		 pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE )
	{
		return SHADOWS_NONE;
	}

	return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
}

float g_flFattenAmt = 4;
void C_TFCSPlayer::GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType )
{
	if ( shadowType == SHADOWS_SIMPLE )
	{
		// Don't let the render bounds change when we're using blobby shadows, or else the shadow
		// will pop and stretch.
		mins = CollisionProp()->OBBMins();
		maxs = CollisionProp()->OBBMaxs();
	}
	else
	{
		GetRenderBounds( mins, maxs );

		// We do this because the normal bbox calculations don't take pose params into account, and 
		// the rotation of the guy's upper torso can place his gun a ways out of his bbox, and 
		// the shadow will get cut off as he rotates.
		//
		// Thus, we give it some padding here.
		mins -= Vector( g_flFattenAmt, g_flFattenAmt, 0 );
		maxs += Vector( g_flFattenAmt, g_flFattenAmt, 0 );
	}
}


void C_TFCSPlayer::GetRenderBounds( Vector& theMins, Vector& theMaxs )
{
	// TODO POSTSHIP - this hack/fix goes hand-in-hand with a fix in CalcSequenceBoundingBoxes in utils/studiomdl/simplify.cpp.
	// When we enable the fix in CalcSequenceBoundingBoxes, we can get rid of this.
	//
	// What we're doing right here is making sure it only uses the bbox for our lower-body sequences since,
	// with the current animations and the bug in CalcSequenceBoundingBoxes, are WAY bigger than they need to be.
	C_BaseAnimating::GetRenderBounds( theMins, theMaxs );
}


bool C_TFCSPlayer::GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const
{
	if ( shadowType == SHADOWS_SIMPLE )
	{
		// Blobby shadows should sit directly underneath us.
		pDirection->Init( 0, 0, -1 );
		return true;
	}
	else
		return BaseClass::GetShadowCastDirection( pDirection, shadowType );
}

bool C_TFCSPlayer::ShouldReceiveProjectedTextures( int flags )
{
	Assert( flags & SHADOW_FLAGS_PROJECTED_TEXTURE_TYPE_MASK );

	if ( IsEffectActive( EF_NODRAW ) )
		return false;

	if ( flags & SHADOW_FLAGS_FLASHLIGHT )
		return true;

	return BaseClass::ShouldReceiveProjectedTextures( flags );
}

CStudioHdr * C_TFCSPlayer::OnNewModel( void )
{
	CStudioHdr *pHdr = BaseClass::OnNewModel();
	if ( pHdr )
	{
		InitPlayerGibs();

		//if ( GetAnimState() )
		//	GetAnimState()->OnNewModel();
	}

	return pHdr;
}

void C_TFCSPlayer::DoImpactEffect( trace_t& tr, int nDamageType )
{
	if ( GetActiveWeapon() )
	{
		GetActiveWeapon()->DoImpactEffect( tr, nDamageType );
		return;
	}

	BaseClass::DoImpactEffect( tr, nDamageType );
}

bool C_TFCSPlayer::ShouldDraw( void )
{
	// If we're dead, our ragdoll will be drawn for us instead.
	if ( !IsAlive() )
		return false;

	if( GetTeamNumber() == TEAM_SPECTATOR )
		return false;

	if( IsLocalPlayer() && IsRagdoll() )
		return true;
	
	if ( IsRagdoll() )
		return false;

	return BaseClass::ShouldDraw();
}

Vector C_TFCSPlayer::GetObserverCamOrigin( void )
{
	if ( !IsAlive() )
	{
		if ( m_hFirstGib )
		{
			IPhysicsObject *pPhysicsObject = m_hFirstGib->VPhysicsGetObject();
			if ( pPhysicsObject )
			{
				Vector vecMassCenter = pPhysicsObject->GetMassCenterLocalSpace();
				Vector vecWorld;
				m_hFirstGib->CollisionProp()->CollisionToWorldSpace( vecMassCenter, &vecWorld );
				return ( vecWorld );
			}
			return m_hFirstGib->GetRenderOrigin();
		}

		IRagdoll *pRagdoll = GetRepresentativeRagdoll();
		if ( pRagdoll )
			return pRagdoll->GetRagdollOrigin();
	}

	return BaseClass::GetObserverCamOrigin();
}

const QAngle& C_TFCSPlayer::GetRenderAngles()
{
	if ( IsRagdoll() || !GetAnimState() )
		return vec3_angle;
	else
		return GetAnimState()->GetRenderAngles();
}

const QAngle& C_TFCSPlayer::EyeAngles()
{
	if ( IsLocalPlayer() )
		return BaseClass::EyeAngles();
	else
		return m_angEyeAngles;
}

void C_TFCSPlayer::UpdateClientSideAnimation()
{
	// Update the animation data. It does the local check here so this works when using
	// a third-person camera (and we don't have valid player angles).
	if ( GetAnimState() )
	{
		if ( this == C_BasePlayer::GetLocalPlayer() )
			GetAnimState()->Update( EyeAngles()[YAW], m_angEyeAngles[PITCH] );
		else
			GetAnimState()->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );	
	}


	BaseClass::UpdateClientSideAnimation();
}

void C_TFCSPlayer::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
		SetNextClientThink( CLIENT_THINK_ALWAYS );

	UpdateVisibility();
}

void C_TFCSPlayer::ItemPreFrame( void )
{
	if ( GetFlags() & FL_FROZEN )
		 return;

	BaseClass::ItemPreFrame();

}
	
void C_TFCSPlayer::ItemPostFrame( void )
{
	if ( GetFlags() & FL_FROZEN )
		 return;

	BaseClass::ItemPostFrame();
}

void C_TFCSPlayer::PostDataUpdate( DataUpdateType_t updateType )
{
	// C_BaseEntity assumes we're networking the entity's angles, so pretend that it
	// networked the same value we already have.
	SetNetworkAngles( GetLocalAngles() );

	// Did we just respawn?
	if ( m_bSpawnInterpCounter != m_bSpawnInterpCounterCache )
		Respawn();

	BaseClass::PostDataUpdate( updateType );
}

void C_TFCSPlayer::Respawn()
{
	// fix up interp
	MoveToLastReceivedPosition( true );
	ResetLatched();

	m_bSpawnInterpCounterCache = m_bSpawnInterpCounter;

	RemoveAllDecals();

	if ( GetAnimState() )
		GetAnimState()->ClearAnimationState();
	
	// reset HDR
	if ( IsLocalPlayer() )
		ResetToneMapping( 1.0 );

	m_hFirstGib = NULL;
	m_hSpawnedGibs.Purge();
}


// Purpose: Update this client's target entity
void C_TFCSPlayer::UpdateIDTarget()
{
	if ( !IsLocalPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show id's in any state but active.
	//if ( State_Get() != STATE_ACTIVE )
	//	return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10,   MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && ( pEntity != this ) )
			m_iIDEntIndex = pEntity->entindex();
	}
}

void C_TFCSPlayer::InitPlayerGibs( void )
{
	// Clear out the gib list and create a new one.
	m_aGibs.Purge();
	BuildGibList( m_aGibs, GetModelIndex(), 1.0f, COLLISION_GROUP_NONE );
}

bool C_TFCSPlayer::CreatePlayerGibs( const Vector & vecOrigin, const Vector & vecVelocity, float flImpactScale, bool bBurning )
{
	// Make sure we have Gibs to create.
	if ( m_aGibs.Count() == 0 )
		return false;

	AngularImpulse angularImpulse( RandomFloat( 0.0f, 120.0f ), RandomFloat( 0.0f, 120.0f ), 0.0 );

	Vector vecBreakVelocity = vecVelocity;
	vecBreakVelocity.z += cl_playergib_forceup.GetFloat();
	VectorNormalize( vecBreakVelocity );
	vecBreakVelocity *= cl_playergib_force.GetFloat();

	// Cap the impulse.
	float flSpeed = vecBreakVelocity.Length();
	if ( flSpeed > cl_playergib_maxspeed.GetFloat() )
		VectorScale( vecBreakVelocity, cl_playergib_maxspeed.GetFloat() / flSpeed, vecBreakVelocity );

	breakablepropparams_t breakParams( vecOrigin, GetRenderAngles(), vecBreakVelocity, angularImpulse );
	breakParams.impactEnergyScale = 1.0f;//

	// Break up the player.
	m_hSpawnedGibs.Purge();
	m_hFirstGib = CreateGibsFromList( m_aGibs, GetModelIndex(), NULL, breakParams, this, -1, false, true, &m_hSpawnedGibs, bBurning );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Try to steer away from any players and objects we might interpenetrate
//-----------------------------------------------------------------------------
#define TFCS_AVOID_MAX_RADIUS_SQR		5184.0f			// Based on player extents and max buildable extents.
#define TFCS_OO_AVOID_MAX_RADIUS_SQR		0.00029f

void C_TFCSPlayer::AvoidPlayers( CUserCmd *pCmd )
{
	// Don't test if the player doesn't exist or is dead.
	if ( IsAlive() == false )
		return;

	C_Team *pTeam = ( C_Team * )GetTeam();
	if ( !pTeam )
		return;

	// Up vector.
	static Vector vecUp( 0.0f, 0.0f, 1.0f );

	Vector vecTFCSPlayerCenter = GetAbsOrigin();
	Vector vecTFCSPlayerMin = GetPlayerMins();
	Vector vecTFCSPlayerMax = GetPlayerMaxs();
	float flZHeight = vecTFCSPlayerMax.z - vecTFCSPlayerMin.z;
	vecTFCSPlayerCenter.z += 0.5f * flZHeight;
	VectorAdd( vecTFCSPlayerMin, vecTFCSPlayerCenter, vecTFCSPlayerMin );
	VectorAdd( vecTFCSPlayerMax, vecTFCSPlayerCenter, vecTFCSPlayerMax );

	// Find an intersecting player or object.
	int nAvoidPlayerCount = 0;
	C_TFCSPlayer *pAvoidPlayerList[MAX_PLAYERS];

	C_TFCSPlayer *pIntersectPlayer = NULL;
	float flAvoidRadius = 0.0f;

	Vector vecAvoidCenter, vecAvoidMin, vecAvoidMax;
	for ( int i = 0; i < pTeam->GetNumPlayers(); ++i )
	{
		C_TFCSPlayer *pAvoidPlayer = ToTFCSPlayer( pTeam->GetPlayer( i ) );
		if ( pAvoidPlayer == NULL )
			continue;
		// Is the avoid player me?
		if ( pAvoidPlayer == this )
			continue;

		// Save as list to check against for objects.
		pAvoidPlayerList[nAvoidPlayerCount] = pAvoidPlayer;
		++nAvoidPlayerCount;

		// Check to see if the avoid player is dormant.
		if ( pAvoidPlayer->IsDormant() )
			continue;

		// Is the avoid player solid?
		if ( pAvoidPlayer->IsSolidFlagSet( FSOLID_NOT_SOLID ) )
			continue;

		Vector t1, t2;

		vecAvoidCenter = pAvoidPlayer->GetAbsOrigin();
		vecAvoidMin = pAvoidPlayer->GetPlayerMins();
		vecAvoidMax = pAvoidPlayer->GetPlayerMaxs();
		flZHeight = vecAvoidMax.z - vecAvoidMin.z;
		vecAvoidCenter.z += 0.5f * flZHeight;
		VectorAdd( vecAvoidMin, vecAvoidCenter, vecAvoidMin );
		VectorAdd( vecAvoidMax, vecAvoidCenter, vecAvoidMax );

		if ( IsBoxIntersectingBox( vecTFCSPlayerMin, vecTFCSPlayerMax, vecAvoidMin, vecAvoidMax ) )
		{
			// Need to avoid this player.
			if ( !pIntersectPlayer )
			{
				pIntersectPlayer = pAvoidPlayer;
				break;
			}
		}
	}

	// Anything to avoid?
	if ( !pIntersectPlayer )
		return;

	// Calculate the push strength and direction.
	Vector vecDelta;

	// Avoid a player - they have precedence.
	if ( pIntersectPlayer )
	{
		VectorSubtract( pIntersectPlayer->WorldSpaceCenter(), vecTFCSPlayerCenter, vecDelta );

		Vector vRad = pIntersectPlayer->WorldAlignMaxs() - pIntersectPlayer->WorldAlignMins();
		vRad.z = 0;

		flAvoidRadius = vRad.Length();
	}

	float flPushStrength = RemapValClamped( vecDelta.Length(), flAvoidRadius, 0, 0, cl_max_separation_force.GetInt() ); //flPushScale;

#ifdef DEBUG
	DevMsg( "PushScale = %f\n", flPushStrength );  
#endif // DEBUG


	// Check to see if we have enough push strength to make a difference.
	if ( flPushStrength < 0.01f )
		return;

	Vector vecPush;
	if ( GetAbsVelocity().Length2DSqr() > 0.1f )
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z = 0.0f;
		CrossProduct( vecUp, vecVelocity, vecPush );
		VectorNormalize( vecPush );
	}
	else
	{
		// We are not moving, but we're still intersecting.
		QAngle angView = pCmd->viewangles;
		angView.x = 0.0f;
		AngleVectors( angView, NULL, &vecPush, NULL );
	}

	// Move away from the other player/object.
	Vector vecSeparationVelocity;
	if ( vecDelta.Dot( vecPush ) < 0 )
		vecSeparationVelocity = vecPush * flPushStrength;
	else
		vecSeparationVelocity = vecPush * -flPushStrength;

	// Don't allow the max push speed to be greater than the max player speed.
	float flMaxPlayerSpeed = MaxSpeed();
	float flCropFraction = 1.33333333f;

	if ( ( GetFlags() & FL_DUCKING ) && ( GetGroundEntity() != NULL ) )
		flMaxPlayerSpeed *= flCropFraction;

	float flMaxPlayerSpeedSqr = flMaxPlayerSpeed * flMaxPlayerSpeed;

	if ( vecSeparationVelocity.LengthSqr() > flMaxPlayerSpeedSqr )
	{
		vecSeparationVelocity.NormalizeInPlace();
		VectorScale( vecSeparationVelocity, flMaxPlayerSpeed, vecSeparationVelocity );
	}

	QAngle vAngles = pCmd->viewangles;
	vAngles.x = 0;
	Vector currentdir;
	Vector rightdir;

	AngleVectors( vAngles, &currentdir, &rightdir, NULL );

	Vector vDirection = vecSeparationVelocity;

	VectorNormalize( vDirection );

	float fwd = currentdir.Dot( vDirection );
	float rt = rightdir.Dot( vDirection );

	float forward = fwd * flPushStrength;
	float side = rt * flPushStrength;

#ifdef DEBUG
	DevMsg( "fwd: %f - rt: %f - forward: %f - side: %f\n", fwd, rt, forward, side );  
#endif // DEBUG


	pCmd->forwardmove	+= forward;
	pCmd->sidemove		+= side;

	// Clamp the move to within legal limits, preserving direction. This is a little
	// complicated because we have different limits for forward, back, and side
#ifdef DEBUG
	DevMsg( "PRECLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );
#endif

	float flForwardScale = 1.0f;
	if ( pCmd->forwardmove > fabs( cl_forwardspeed.GetFloat() ) )
		flForwardScale = fabs( cl_forwardspeed.GetFloat() ) / pCmd->forwardmove;
	else if ( pCmd->forwardmove < -fabs( cl_backspeed.GetFloat() ) )
		flForwardScale = fabs( cl_backspeed.GetFloat() ) / fabs( pCmd->forwardmove );

	float flSideScale = 1.0f;
	if ( fabs( pCmd->sidemove ) > fabs( cl_sidespeed.GetFloat() ) )
		flSideScale = fabs( cl_sidespeed.GetFloat() ) / fabs( pCmd->sidemove );

	float flScale = min( flForwardScale, flSideScale );
	pCmd->forwardmove *= flScale;
	pCmd->sidemove *= flScale;

#ifdef DEBUG
	DevMsg( "Pforwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );  
#endif // DEBUG

}

bool C_TFCSPlayer::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{	
	static QAngle angMoveAngle( 0.0f, 0.0f, 0.0f );

	VectorCopy( pCmd->viewangles, angMoveAngle );

	BaseClass::CreateMove( flInputSampleTime, pCmd );

	AvoidPlayers( pCmd );

	return true;
}
