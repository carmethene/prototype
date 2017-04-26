//------------------------------------------------------------------------------
// File: ObjectManager.h
// Desc: Looks after all objects in the game
//
// Created: 14 September 2005 16:18:19
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTMANAGER_H
#define INCLUSIONGUARD_OBJECTMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Object.h"
#include <map>
#include <list>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
struct ObjectType
{
	unsigned int		MeshNumber;
	float				MeshScale;

	float				Mass;
	float				ThrustMagnitude;
	float				MaxVelocity;

	float				MaxHealth;
	float				CollisionDamage;

	unsigned int		KillScore;
};


class GObjectManager
{
public:
	friend class Object;

	static bool				Initialise();
	static bool				Shutdown();

	static bool				DeleteAllObjects();
	static bool				ObliterateLists();	//Complete delete/clear without usage of the deadlist

	static bool				ProcessObjects();
	static bool				RenderObjects( float frame_delta );

	static Object*			CreateObject( OBJECT_TYPE object_type, unsigned int object_number, unsigned int player_number, D3DXVECTOR2 pos, float orientation );

	static unsigned int		GetObjectCount()				{ return unsigned int(Objects.size()); }

private:
	static bool				LoadObjectTypes();

	static bool				IsObjectOffscreen( Object *obj );
	static bool				DoObjectsCollide( Object *obj1, Object *obj2 );

	static bool				StartLights();
	static bool				StopLights();

	typedef		std::map< unsigned int, ObjectType >		ObjectTypeMap;
	typedef		std::list< Object* >						ObjectList;

	static ObjectTypeMap	ObjectTypes;
	static ObjectList		Objects;
	static ObjectList		DeletedObjects;

	static bool				Initialised;
};

#endif //INCLUSIONGUARD_OBJECTMANAGER_H
