//------------------------------------------------------------------------------
// File: PlayerComputer.h
// Desc: Representation of a computerised player
//
// Created: 15 September 2005 17:00:22
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PLAYERCOMPUTER_H
#define INCLUSIONGUARD_PLAYERCOMPUTER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Player.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class PlayerComputer : public Player
{
public:
	PlayerComputer();
	~PlayerComputer();

	bool						Initialise( const char *filename );
	bool						Shutdown();

	bool						Process();

protected:
	unsigned int				Timer1;
	unsigned int				Timer2;

};


#endif //INCLUSIONGUARD_PLAYERCOMPUTER_H
