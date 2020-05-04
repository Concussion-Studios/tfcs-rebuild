//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TFC specific input handling
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "kbutton.h"
#include "input.h"

//-----------------------------------------------------------------------------
// Purpose: TFC Input interface
//-----------------------------------------------------------------------------
class CTFCInput : public CInput
{
public:
};

static CTFCInput g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;

