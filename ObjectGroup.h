//------------------------------------------------------------------------------
// File: ObjectGroup.h
// Desc: System to move a set of objects in a predetermined pattern
//
// Created: 27 September 2005 16:53:56
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTGROUP_H
#define INCLUSIONGUARD_OBJECTGROUP_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>
#include <list>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Object;


enum GROUP_TYPE
{
	GROUP_TYPE_STRAIGHT,
	GROUP_TYPE_SINE,
	GROUP_TYPE_CIRCLE,
};


class ObjectGroup
{
	friend class GGroupManager;

public:
	bool				HasBeenDeleted()				{ return Deleted; }

protected:
	ObjectGroup();
	virtual ~ObjectGroup();

	unsigned int		GetNumObjects();

	virtual bool		InitialiseObjectMovement() = 0;
	virtual bool		ProcessObjectMovement() = 0;

	typedef			std::list< Object* >		ObjectList;

	ObjectList			Objects;

	D3DXVECTOR2			Position;
	D3DXVECTOR2			Direction;

private:
	bool				AddObject( Object *object );

	bool				Process();
	bool				ProcessObjectStatus();

	bool				Deleted;

};


#endif //INCLUSIONGUARD_OBJECTGROUP_H
