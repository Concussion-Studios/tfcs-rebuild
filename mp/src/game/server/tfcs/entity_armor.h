//TFCS Armor
//TODO: Armor skins for the different types
#ifndef ENTITY_ARMOR_H
#define ENTITY_ARMOR_H

#ifdef _WIN32
#pragma once
#endif

#include "items.h"

#define ARMOR_MODEL			"models/items/car_battery01.mdl"
#define ARMOR_PICKUP_SOUND	"Armor.Touch"
#define ARMOR_RESPAWN_TIME	20.0f

#define ARMOR1_CLASS 0.3
#define ARMOR2_CLASS 0.6
#define ARMOR3_CLASS 0.8

#define ARMOR1_COUNT 100
#define ARMOR2_COUNT 150
#define ARMOR3_COUNT 200

//For engineers to get metal from armor
#define ARMOR1_CELLS 50
#define ARMOR2_CELLS 100
#define ARMOR3_CELLS 150

class CEntityArmor : public CItem
{
public:
	DECLARE_CLASS( CEntityArmor, CItem );

	void Spawn( void );
	void Precache( void );
	bool MyTouch( CBasePlayer *pPlayer );
	virtual float GetArmorClass() { return 0.0f; }
	virtual int GetArmorCount() { return 0; }
	virtual int GetArmorMetal() { return 0; }
};

//Green/Light Armor
class CEntityArmor1 : public CEntityArmor
{
public:
	DECLARE_CLASS( CEntityArmor1, CEntityArmor );
	virtual float GetArmorClass() { return ARMOR1_CLASS; }
	virtual int GetArmorCount() { return ARMOR1_COUNT; }
	virtual int GetArmorMetal() { return ARMOR1_CELLS; }
};

//Yellow/Medium Armor
class CEntityArmor2 : public CEntityArmor
{
public:
	DECLARE_CLASS( CEntityArmor2, CEntityArmor );
	virtual float GetArmorClass() { return ARMOR2_CLASS; }
	virtual int GetArmorCount() { return ARMOR2_COUNT; }
	virtual int GetArmorMetal() { return ARMOR2_CELLS; }
};

//Red/Heavy Armor
class CEntityArmor3 : public CEntityArmor
{
public:
	DECLARE_CLASS( CEntityArmor3, CEntityArmor );
	virtual float GetArmorClass() { return ARMOR3_CLASS; }
	virtual int GetArmorCount() { return ARMOR3_COUNT; }
	virtual int GetArmorMetal() { return ARMOR3_CELLS; }
};
#endif //ENTITY_ARMOR_H