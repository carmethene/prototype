//------------------------------------------------------------------------------
// File: ObjectGroupStraight.cpp
// Desc: Makes objects move in a straight line down the screen
//
// Created: 27 September 2005 18:26:27
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroupStraight.h"
#include "Object.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectGroupStraight::ObjectGroupStraight()
{

}

//------------------------------------------------------------------------------------------------------------
ObjectGroupStraight::~ObjectGroupStraight()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupStraight::InitialiseObjectMovement()
{
	if( Objects.size() == 0 )
	{
		OutputDebugStringA( "PT_ERROR: No objects in group\n" );
		return false;
	}

	//Line up the objects so they fly through the start position
	D3DXVECTOR2 pos = Position;
	D3DXVECTOR2 sep = Direction * (*(Objects.begin()))->GetCollisionRadius() * 2.0f;

	for( ObjectList::iterator iter = Objects.begin(); iter != Objects.end(); ++iter )
	{
		Object *object = *iter;
		object->SetPosition( pos );
		pos -= sep;

		object->SetThrust( Direction );
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupStraight::ProcessObjectMovement()
{
	return true;
}
