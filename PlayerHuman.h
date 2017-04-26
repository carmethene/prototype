//------------------------------------------------------------------------------
// File: PlayerHuman.h
// Desc: Representation of a human player
//
// Created: 15 September 2005 17:00:32
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PLAYERHUMAN_H
#define INCLUSIONGUARD_PLAYERHUMAN_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Player.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Object;

class PlayerHuman : public Player
{
public:
	PlayerHuman();
	~PlayerHuman();

	bool						Initialise( const char *filename );
	bool						Shutdown();

	bool						Process();

	Object*						GetMainObject()									{ return ComputerControlsShip ? 0 : Ship; }

private:
	bool						ProcessShipStatus();
	bool						ProcessShipMovement();
	bool						ProcessShipWeapon();

	unsigned int				ComputerControlTime;
	bool						ComputerControlsShip;

	Object						*Ship;

};


#endif //INCLUSIONGUARD_PLAYERHUMAN_H
