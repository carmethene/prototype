//------------------------------------------------------------------------------
// File: GroupManager.h
// Desc: Looks after object groups
//
// Created: 27 September 2005 17:42:06
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_GROUPMANAGER_H
#define INCLUSIONGUARD_GROUPMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroup.h"
#include "Object.h"
#include <list>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GGroupManager
{
public:
	static bool					Initialise();
	static bool					Shutdown();

	static bool					Process();

	static ObjectGroup*			CreateGroup( GROUP_TYPE group_type, D3DXVECTOR2 pos, D3DXVECTOR2 direction, unsigned int object_count, OBJECT_TYPE object_type, unsigned int object_number, unsigned int player_number );

	static bool					DeleteAllGroups();

private:
	typedef				std::list< ObjectGroup* >			GroupList;

	static GroupList			Groups;
	static GroupList			DeletedGroups;

	static bool					Initialised;
};


#endif //INCLUSIONGUARD_GROUPMANAGER_H
