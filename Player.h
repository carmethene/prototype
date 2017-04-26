//------------------------------------------------------------------------------
// File: Player.h
// Desc: Base class for players
//
// Created: 15 September 2005 17:22:02
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PLAYER_H
#define INCLUSIONGUARD_PLAYER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Object;


class Player
{
public:
	Player();
	virtual ~Player();

	virtual bool				Initialise( const char *filename ) = 0;
	virtual bool				Shutdown() = 0;

	virtual bool				Process() = 0;

	void						SetPlayerNumber( unsigned int number )			{ PlayerNumber = number; }

	unsigned int				GetLives()										{ return Lives; }
	unsigned int				GetScore()										{ return Score; }
	float						GetHealth()										{ return Health; }

	void						AddToScore( unsigned int value )				{ Score += value; }

	void						SetTarget( Object *target )						{ Target = target; }
	Object*						GetTarget()										{ return Target; }

	virtual Object*				GetMainObject()									{ return 0; }

protected:
	unsigned int				PlayerNumber;

	unsigned int				Lives;
	unsigned int				Score;
	float						Health;

	Object						*Target;

};

#endif //INCLUSIONGUARD_PLAYER_H
