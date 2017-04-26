//------------------------------------------------------------------------------
// File: ObjectShipComputer.h
// Desc: A computer-controlled ship
//
// Created: 19 September 2005 12:11:25
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTSHIPCOMPUTER_H
#define INCLUSIONGUARD_OBJECTSHIPCOMPUTER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Object.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectShipComputer : public Object
{
public:
	ObjectShipComputer();
	~ObjectShipComputer();

	virtual void		ReduceHealth( float amount );

protected:
	virtual bool		Process();
	virtual bool		ProcessBounds();

	unsigned int		FireTimer;
};


#endif //INCLUSIONGUARD_OBJECTSHIPCOMPUTER_H
