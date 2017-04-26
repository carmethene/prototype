//------------------------------------------------------------------------------
// File: Object.h
// Desc: An object in the game world
//
// Created: 14 September 2005 15:34:40
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECT_H
#define INCLUSIONGUARD_OBJECT_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
enum OBJECT_TYPE
{
	OBJECT_TYPE_SHIP_HUMAN,
	OBJECT_TYPE_SHIP_COMPUTER,
	OBJECT_TYPE_AMMO,
	OBJECT_TYPE_PICKUP
};


class Object
{
public:
	friend class GObjectManager;

	bool				HasBeenDeleted()					{ return Deleted; }

	D3DXVECTOR2			GetPosition()						{ return Position; }
	float				GetCollisionRadius()				{ return CollisionRadius; }

	float				GetMaxHealth()						{ return MaxHealth; }
	float				GetHealth()							{ return Health; }

	void				SetPosition( D3DXVECTOR2 pos )		{ Position = pos; }

	//Takes a normalised thrust direction vector
	void				SetThrust( D3DXVECTOR2 thrust )		{ Thrust = thrust; }

	void				SetIndestructable( bool value )		{ Indestructable = value; }
	virtual void		ReduceHealth( float amount );

protected:
	Object();
	virtual ~Object();

	virtual bool		Process();
	virtual bool		ProcessPhysics();
	virtual bool		ProcessBounds();

	virtual bool		Render( float frame_delta );

	OBJECT_TYPE			ObjectType;

	unsigned int		PlayerNumber;

	D3DXVECTOR2			Position;
	D3DXVECTOR2			OldPosition;
	float				Orientation;
	float				OldOrientation;
	float				Roll;
	float				OldRoll;

	D3DXVECTOR2			Velocity;
	D3DXVECTOR2			Thrust;		//Normalised thrust direction
	float				Mass;
	float				ThrustMagnitude;
	float				MaxVelocity;

	unsigned int		MeshNumber;
	float				MeshScale;

	float				MaxHealth;
	float				Health;
	float				CollisionDamage;
	float				CollisionRadius;

	unsigned int		KillScore;

	bool				Indestructable;
	bool				Deleted;
};


#endif //INCLUSIONGUARD_OBJECT_H
