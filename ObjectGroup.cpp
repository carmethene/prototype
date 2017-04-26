//------------------------------------------------------------------------------
// File: ObjectGroup.cpp
// Desc: System to move a set of objects in a predetermined pattern
//
// Created: 27 September 2005 16:54:15
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroup.h"
#include "Object.h"
#include <algorithm>
#include <cassert>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectGroup::ObjectGroup()
{
	Position = D3DXVECTOR2( 0.0f, 0.0f );
	Direction = D3DXVECTOR2( 0.0f, 0.0f );

	Deleted = false;
}

//------------------------------------------------------------------------------------------------------------
ObjectGroup::~ObjectGroup()
{

}

//------------------------------------------------------------------------------------------------------------
bool					ObjectGroup::Process()
{
	if( ! ProcessObjectStatus() )
	{
		return false;
	}

	if( ! ProcessObjectMovement() )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					ObjectGroup::AddObject( Object *object )
{
	assert( std::find( Objects.begin(), Objects.end(), object ) == Objects.end() );

	Objects.push_back( object );

	return true;
}

//------------------------------------------------------------------------------------------------------------
unsigned int			ObjectGroup::GetNumObjects()
{
	return unsigned int( Objects.size() );
}

//------------------------------------------------------------------------------------------------------------
bool					ObjectGroup::ProcessObjectStatus()
{
	//If any objects have been deleted, they need to be removed from the list
	ObjectList::iterator iter = Objects.begin();
	while( iter != Objects.end() )
	{
		Object *object = *iter;
		if( object->HasBeenDeleted() )
		{
			iter = Objects.erase( iter );
			continue;
		}

		++iter;
	}

	return true;
}

