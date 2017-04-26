//------------------------------------------------------------------------------
// File: GroupManager.cpp
// Desc: Looks after object groups
//
// Created: 27 September 2005 17:42:05
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "GroupManager.h"
#include "ObjectGroupStraight.h"
#include "ObjectGroupSine.h"
#include "ObjectGroupCircle.h"
#include "ObjectManager.h"
#include <new>
#include <cassert>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GGroupManager::GroupList		GGroupManager::Groups;
GGroupManager::GroupList		GGroupManager::DeletedGroups;
bool							GGroupManager::Initialised = false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool					GGroupManager::Initialise()
{
	assert( ! Initialised );

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GGroupManager::Shutdown()
{
	assert( Initialised );

	//Delete all groups
	for( GroupList::iterator iter = Groups.begin(); iter != Groups.end(); ++iter )
	{
		ObjectGroup *group = *iter;
		delete group;
	}
	Groups.clear();

	for( GroupList::iterator iter = DeletedGroups.begin(); iter != DeletedGroups.end(); ++iter )
	{
		ObjectGroup *group = *iter;
		delete group;
	}
	DeletedGroups.clear();

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GGroupManager::Process()
{
	assert( Initialised );

	bool retval = true;

	//Empty the deadlist
	for( GroupList::iterator iter = DeletedGroups.begin(); iter != DeletedGroups.end(); ++iter )
	{
		ObjectGroup *group = *iter;
		delete group;
	}
	DeletedGroups.clear();

	//Process the groups
	GroupList::iterator iter2 = Groups.begin();
	while( iter2 != Groups.end() )
	{
		ObjectGroup *group = *iter2;

		if( ! group->Process() )
		{
			retval = false;
		}

		if( group->GetNumObjects() == 0 )
		{
			//Group is empty - delete it!
			DeletedGroups.push_back( group );
			iter2 = Groups.erase( iter2 );
			group->Deleted = true;
			continue;
		}

		++iter2;
	}

	return retval;
}

//------------------------------------------------------------------------------------------------------------
ObjectGroup*			GGroupManager::CreateGroup( GROUP_TYPE group_type, D3DXVECTOR2 pos, D3DXVECTOR2 direction, unsigned int object_count, OBJECT_TYPE object_type, unsigned int object_number, unsigned int player_number )
{
	assert( Initialised );

	//Create the group
	ObjectGroup *object_group = NULL;
	switch( group_type )
	{
	case GROUP_TYPE_STRAIGHT:
		object_group = new(std::nothrow) ObjectGroupStraight;
		break;

	case GROUP_TYPE_SINE:
		object_group = new(std::nothrow) ObjectGroupSine;
		break;

	case GROUP_TYPE_CIRCLE:
		object_group = new(std::nothrow) ObjectGroupCircle;
		break;

	default:
		OutputDebugStringA( "PT_ERROR: Unknown group type\n" );
		return NULL;
	};

	if( ! object_group )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory\n" );
		return NULL;
	}

	//Set the parameters
	object_group->Position = pos;
	object_group->Direction = direction;

	//Work out the orientation
	float orientation = 0.0f;
	if( direction.y < 0.0f )
	{
        orientation = D3DX_PI;
	}

	//Create the objects
	for( unsigned int i = 0; i < object_count; ++i )
	{
		Object *object = GObjectManager::CreateObject( object_type, object_number, player_number, pos, orientation );
		if( object )
		{
			object_group->AddObject( object );
		}
	}

	//Initialise the objects
	object_group->InitialiseObjectMovement();

	//Store and return
	Groups.push_back( object_group );

	return object_group;
}

//------------------------------------------------------------------------------------------------------------
bool					GGroupManager::DeleteAllGroups()
{
    assert( Initialised );

	for( GroupList::iterator iter = Groups.begin(); iter != Groups.end(); ++iter )
	{
		ObjectGroup *group = *iter;
		DeletedGroups.push_back( group );
	}
	Groups.clear();

	return true;
}