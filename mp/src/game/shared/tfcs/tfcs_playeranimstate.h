#ifndef TFCS_PLAYERANIMSTATE_H
#define TFCS_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "iplayeranimstate.h"

#ifdef CLIENT_DLL
class C_TFCSPlayer;
#define CTFCSPlayer C_TFCSPlayer
#else
class CTFCSPlayer;
#endif

#endif //TFCS_PLAYERANIMSTATE_H