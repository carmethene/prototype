//------------------------------------------------------------------------------
// File: ObjectManager.cpp
// Desc: Looks after all objects in the game
//
// Created: 14 September 2005 16:19:33
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "ObjectManager.h"
#include "ObjectShipHuman.h"
#include "ObjectShipComputer.h"
#include "ObjectAmmo.h"
#include "ObjectPickup.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "PlayerManager.h"
#include "Player.h"
#include "Constants.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <new>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GObjectManager::ObjectTypeMap			GObjectManager::ObjectTypes;
GObjectManager::ObjectList				GObjectManager::Objects;
GObjectManager::ObjectList				GObjectManager::DeletedObjects;
bool									GObjectManager::Initialised = false;

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool					GObjectManager::Initialise()
{
	assert( ! Initialised );

	if( ! LoadObjectTypes() )
	{
		return false;
	}

	Initialised = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::Shutdown()
{
	assert( Initialised );

	ObjectTypes.clear();
	ObliterateLists();

	Initialised = false;
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::DeleteAllObjects()
{
	assert( Initialised );

	for( ObjectList::iterator obj_iter = Objects.begin(); obj_iter != Objects.end(); ++obj_iter )
	{
		Object *obj = *obj_iter;
		DeletedObjects.push_back( obj );
	}
	Objects.clear();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::ObliterateLists()
{
	assert( Initialised );

	for( ObjectList::iterator obj_iter = Objects.begin(); obj_iter != Objects.end(); ++obj_iter )
	{
		Object *obj = *obj_iter;
		delete obj;
	}
	Objects.clear();

	for( ObjectList::iterator delobj_iter = DeletedObjects.begin(); delobj_iter != DeletedObjects.end(); ++delobj_iter )
	{
		Object *obj = *delobj_iter;
		delete obj;
	}
	DeletedObjects.clear();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::ProcessObjects()
{
	assert( Initialised );

	//Empty the deleted objects list
	for( ObjectList::iterator del_obj_iter = DeletedObjects.begin(); del_obj_iter != DeletedObjects.end(); ++del_obj_iter )
	{
		Object *obj = *del_obj_iter;
		delete obj;
	}
	DeletedObjects.clear();

	//Update each object
	ObjectList::iterator this_iter = Objects.begin();
	while( this_iter != Objects.end() )
	{
		Object *this_obj = *this_iter;
		
		//if we are dead or fail to process, mark this object for deletion
		//Note that we still want to process objects with zero health as we render the deadlist
		if( ! this_obj->Process() || this_obj->Health < FLT_EPSILON )
		{
            this_iter = Objects.erase( this_iter );
			DeletedObjects.push_back( this_obj );
			this_obj->Deleted = true;

			continue;
		}

		//See if we have collided with another object
		//Horrible O(n^2) algorithm but there aren't many objects onscreen at once 
		for( ObjectList::iterator enemy_iter = Objects.begin(); enemy_iter != Objects.end(); ++enemy_iter )
		{
			Object *enemy_obj = *enemy_iter;
			if( enemy_obj == this_obj )
			{
				//can't collide with ourself!
				continue;
			}
			else if( enemy_obj->PlayerNumber == this_obj->PlayerNumber )
			{
				//no friendly fire
				continue;
			}
			else if( this_obj < enemy_obj )
			{
				//we will detect two collisions per object so we want to ignore one of them
				continue;
			}

			if( DoObjectsCollide( this_obj, enemy_obj ) )
			{
				//collided!
				this_obj->ReduceHealth( enemy_obj->CollisionDamage );
				enemy_obj->ReduceHealth( this_obj->CollisionDamage );

				if( this_obj->Health <= 0.0f )
				{
					this_obj->CollisionDamage = 0.0f;
				}

				if( enemy_obj->Health <= 0.0f )
				{
					enemy_obj->CollisionDamage = 0.0f;
				}

				//Update scores
				if( this_obj->Health <= 0.0f && this_obj->KillScore > 0 )
				{
					Player *player = GPlayerManager::GetPlayer( enemy_obj->PlayerNumber );
					if( player )
					{
						player->AddToScore( this_obj->KillScore );
						this_obj->KillScore = 0;
					}
				}

				if( enemy_obj->Health <= 0.0f && enemy_obj->KillScore > 0 )
				{
					Player *player = GPlayerManager::GetPlayer( this_obj->PlayerNumber );
					if( player )
					{
						player->AddToScore( enemy_obj->KillScore );
						enemy_obj->KillScore = 0;
					}
				}
			}
		}

		++this_iter;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::RenderObjects( float frame_delta )
{
	assert( Initialised );

	LPDIRECT3DDEVICE9 d3d_device = DXUTGetD3DDevice();
	assert( d3d_device );

	StartLights();

	d3d_device->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	d3d_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
	d3d_device->SetRenderState( D3DRS_STENCILREF, 1 );

	for( ObjectList::iterator iter = Objects.begin(); iter != Objects.end(); ++iter )
	{
		Object *this_obj = *iter;
		if( ! this_obj->Render( frame_delta ) )
		{
			OutputDebugStringA( "PT_ERROR: An object failed to render\n" );
		}
	}
	
	//Note: We still want to render the deadlist or objects will appear to vanish before they actually
	// collide
	for( ObjectList::iterator iter2 = DeletedObjects.begin(); iter2 != DeletedObjects.end(); ++iter2 )
	{
		Object *this_obj = *iter2;
		if( ! this_obj->Render( frame_delta ) )
		{
			OutputDebugStringA( "PT_ERROR: An object failed to render\n" );
		}
	}

	d3d_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	d3d_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
	d3d_device->SetRenderState( D3DRS_STENCILREF, 0 );

	StopLights();

	return true;
}

//------------------------------------------------------------------------------------------------------------
Object*					GObjectManager::CreateObject( OBJECT_TYPE object_type, unsigned int object_number, unsigned int player_number, D3DXVECTOR2 pos, float orientation )
{
	assert( Initialised );

	//retrieve object information
	ObjectTypeMap::const_iterator iter = ObjectTypes.find( object_number );
	if( iter == ObjectTypes.end() )
	{
		std::stringstream error_str;
		error_str << "PT_ERROR: Object #" << object_number << " does not exist\n";
		OutputDebugStringA( error_str.str().c_str() );

		return NULL;
	}
	const ObjectType &type = iter->second;

	//create the object
	Object *object = NULL;
	switch( object_type )
	{
	case OBJECT_TYPE_SHIP_HUMAN:
		object = new(std::nothrow) ObjectShipHuman;
		break;

	case OBJECT_TYPE_SHIP_COMPUTER:
		object = new(std::nothrow) ObjectShipComputer;
		break;

	case OBJECT_TYPE_AMMO:
		object = new(std::nothrow) ObjectAmmo;
		break;

	case OBJECT_TYPE_PICKUP:
		object = new(std::nothrow) ObjectPickup;
		break;

	default:
		OutputDebugStringA( "PT_ERROR: Unknown object type\n" );

		return NULL;
	};

	if( ! object )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory\n" );

		return NULL;
	}

	//assign information to the object
	object->ObjectType		= object_type;
	object->PlayerNumber	= player_number;
	object->Position		= pos;
	object->OldPosition		= pos;
	object->Orientation		= orientation;
	object->OldOrientation	= orientation;

	object->MeshNumber		= type.MeshNumber;
	object->MeshScale		= type.MeshScale;
	object->Mass			= type.Mass;
	object->ThrustMagnitude	= type.ThrustMagnitude;
	object->MaxVelocity		= type.MaxVelocity;
	object->MaxHealth		= type.MaxHealth;
	object->Health			= type.MaxHealth;
	object->CollisionDamage	= type.CollisionDamage;

	Mesh *mesh = GAssetManager::GetMesh( type.MeshNumber );
	if( mesh )
	{
		object->CollisionRadius	= mesh->GetRadius() * type.MeshScale;
	}

	object->KillScore		= type.KillScore;

	Objects.push_back( object );

	return object;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::LoadObjectTypes()
{
	assert( ! Initialised );

	std::ifstream fin( "assets/objects.txt" );
	if( ! fin )
	{
		OutputDebugStringA( "PT_ERROR: Could not open object definition file\n" );
		return false;
	}

	const size_t MAX_LINE = 1024;
	char line_buffer[ MAX_LINE ];
	unsigned int line_count = 1;

	while( fin )
	{
		//get the line
		fin.getline( line_buffer, MAX_LINE );
		std::stringstream line_stream;
		line_stream.str( line_buffer );
		line_stream >> std::ws;
		if( line_stream.str().size() == 0 )
		{
			continue;
		}

		if( line_stream.str()[0] == '#' )
		{
			//this is a comment
			continue;
		}

		//Get details of the object
		unsigned int number;
		ObjectType type;
		memset( &type, 0, sizeof( ObjectType ) );
		line_stream >> number;

		//Check the object hasn't already been defined
		if( ObjectTypes.find( number ) != ObjectTypes.end() )
		{
			std::stringstream error_str;
			error_str << "PT_ERROR: Object #" << number << " has already been defined\n";
			OutputDebugStringA( error_str.str().c_str() );
			continue;
		}

        line_stream >> type.MeshNumber;
		line_stream >> type.MeshScale;
		line_stream >> type.Mass;
		line_stream >> type.ThrustMagnitude;
		line_stream >> type.MaxVelocity;
		line_stream >> type.MaxHealth;
		line_stream >> type.CollisionDamage;
		line_stream >> type.KillScore;

		//Store the object
		ObjectTypes[ number ] = type;

	}
	fin.close();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::IsObjectOffscreen( Object *obj )
{
	if( ! obj )
	{
		return true;
	}

	float x = obj->Position.x;
	float y = obj->Position.y;
	float r = obj->CollisionRadius;

	if( x < -r )
	{
		return true;
	}
	if( x > PT_WORLD_WIDTH + r )
	{
		return true;
	}
	if( y < -r )
	{
		return true;
	}
	if( y > PT_WORLD_HEIGHT + r )
	{
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::DoObjectsCollide( Object *obj1, Object *obj2 )
{
	//Code stolen from: http://www.gamedev.net/reference/articles/article1234.asp
	//due to my laziness :)

	assert( obj1 );
	assert( obj2 );

	//Offscreen objects cannot collide
	if( IsObjectOffscreen( obj1 ) || IsObjectOffscreen( obj2 ) )
	{
		return false;
	}

	const static float time_interval = float(PT_GAME_TICK_INTERVAL_MS) / 1000.0f;

	//Relative velocity
	D3DXVECTOR2 dv = obj2->Velocity - obj1->Velocity;
	dv *= time_interval;

	//Relative position
	D3DXVECTOR2 dp = obj2->Position - obj1->Position;

	//Minimal distance squared
 	float r = obj1->CollisionRadius + obj2->CollisionRadius;

	//dP^2-r^2
	float pp = dp.x * dp.x + dp.y * dp.y - r*r;
	if( pp < 0.0f )
	{
		//Objects are already colliding
		return true;
	}

	//dP*dV
	float pv = dp.x * dv.x + dp.y * dv.y;
	if( pv >= 0.0f )
	{
		//Spheres are moving away from each other
		return false;
	}

	//dV^2
	float vv = dv.x * dv.x + dv.y * dv.y;
	if( (pv + vv) <= 0.0f && (vv + 2.0f * pv + pp) >= 0.0f )
	{
		//Spheres cannot intersect within the next timestep
		return false;
	}

	//Discriminant/4
	float d = pv * pv - pp * vv; 
	return( d > 0.0f );

	/*
	//tmin = -dP*dV/dV*2 
	//the time when the distance between the spheres is minimal
	float tmin = -pv / vv;

	//Discriminant/(4*dV^2) = -(dp^2-r^2+dP*dV*tmin)
	return ( pp + pv * tmin <= 0.0f );
	*/
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::StartLights()
{
	assert( DXUTGetD3DDevice() );

	//Set up fixed lighting
	D3DXVECTOR3 light_pos( (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT-PT_GUI_WIDTH) / 2.0f, -PT_CAMERA_DEPTH );
	D3DXVECTOR3 light_dir( 0.0f, 0.0f, 1.0f );

	D3DLIGHT9 light;
	memset( &light, 0, sizeof( D3DLIGHT9 ) );
	light.Type			= D3DLIGHT_DIRECTIONAL;
	light.Position		= light_pos;
	light.Direction		= light_dir;

	light.Diffuse.a		= 1.0f;
	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Range			= 1000.0f;
	light.Attenuation0	= 0.0f;
	DXUTGetD3DDevice()->SetLight( 0, &light );
	DXUTGetD3DDevice()->LightEnable( 0, TRUE );

	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0xff, 0xff, 0xff, 0xff ) );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GObjectManager::StopLights()
{
	assert( DXUTGetD3DDevice() );

	DXUTGetD3DDevice()->LightEnable( 0, FALSE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

	return true;
}
