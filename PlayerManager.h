//------------------------------------------------------------------------------
// File: PlayerManager.h
// Desc: Maintains player objects
//
// Created: 16 September 2005 13:31:24
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PLAYERMANAGER_H
#define INCLUSIONGUARD_PLAYERMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <map>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Player;


enum PLAYER_TYPE
{
	PLAYER_TYPE_HUMAN,
	PLAYER_TYPE_COMPUTER,
};


class GPlayerManager
{
public:
	static bool					Initialise();
	static bool					Shutdown();

	static bool					CreatePlayer( PLAYER_TYPE type, const char *script_file, unsigned int& player_number );
	static bool					DestroyPlayer( unsigned int player_number );

	static Player*				GetPlayer( unsigned int player_number );

	static bool					Process();

private:
	typedef std::map< unsigned int, Player* >		PlayerMap;

	static PlayerMap			Players;

	static unsigned int			NextPlayerNumber;

	static bool					Initialised;

};

#endif //INCLUSIONGUARD_PLAYERMANAGER_H
