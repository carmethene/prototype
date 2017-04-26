//------------------------------------------------------------------------------
// File: ObjectGroupCircle.cpp
// Desc: Moves objects in a circular pattern
//
// Created: 27 September 2005 20:15:21
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroupCircle.h"
#include "Object.h"


//------------------------------------------------------------------------------
// Globals:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectGroupCircle::ObjectGroupCircle()
{

}

//------------------------------------------------------------------------------------------------------------
ObjectGroupCircle::~ObjectGroupCircle()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupCircle::InitialiseObjectMovement()
{
	if( Objects.size() == 0 )
	{
		OutputDebugStringA( "PT_ERROR: No objects in group\n" );
		return false;
	}

	//TODO

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupCircle::ProcessObjectMovement()
{
	//TODO

	return true;
}
