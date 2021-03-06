/*****************************************************************************
 * Larmor-Physx Version 1.0 2013
 * Copyright (c) 2013 Pier Paolo Ciarravano - http://www.larmor.com
 * All rights reserved.
 *
 * This file is part of Larmor-Physx (http://code.google.com/p/larmor-physx/).
 *
 * Larmor-Physx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Larmor-Physx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Larmor-Physx. If not, see <http://www.gnu.org/licenses/>.
 *
 * Licensees holding a valid commercial license may use this file in
 * accordance with the commercial license agreement provided with the
 * software.
 *
 * Author: Pier Paolo Ciarravano
 * $Id$
 *
 ****************************************************************************/

#ifndef CORE_CLASSES_H_
#define CORE_CLASSES_H_

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <vector>
#include <list>
#include <math.h>
#include <exception>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BT_USE_DOUBLE_PRECISION 1
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

#include <boost/serialization/vector.hpp>
#include <boost/unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "../shatter/DelaunayVoronoi.h"
#include "ConfigManager.h"

#if defined(WIN32) || defined(WIN64)
	//Windows file separator
	#define FILE_SEPARATOR_CHAR "\\"
#elif defined(__APPLE__) || defined(LINUX) || defined(linux)
	//Linux/OSX file separator
	#define FILE_SEPARATOR_CHAR "/"
#endif

#define LReal double

namespace LarmorPhysx
{

	//Simple vector
	class LVector3 {

		public:
			LReal x;
			LReal y;
			LReal z;

			LVector3() {}

			LVector3(const LVector3 &a) : x(a.x), y(a.y), z(a.z) {}

			LVector3(LReal nx, LReal ny, LReal nz) : x(nx), y(ny), z(nz) {}

			LVector3 &operator =(const LVector3 &a) {
				x = a.x; y = a.y; z = a.z;
				return *this;
			}

			bool operator ==(const LVector3 &a) const {
				return x==a.x && y==a.y && z==a.z;
			}

			bool operator !=(const LVector3 &a) const {
				return x!=a.x || y!=a.y || z!=a.z;
			}

	};


	//Static Object
	class StaticObject {

		public:
			unsigned int idObj;
			unsigned int idParentObj; //Id parent shattered object that generated this piece: 0 means that this mesh doesn't come from a shattering
			TranslatedMeshData meshData;
			LReal volume;
			LReal mass;
			LReal density;
			LVector3 inertia;
			LReal friction;
			LReal restitution;
			bool isConvex; //if true use bullet physics btConvexHullShape
			//unsigned int idMaterial; //To store the material id for rendering: actually it could be done using a new TriangleInfo attribute
			LReal breakCoefficient; //specify the volume factor of shattered pieces: number pieces should be = volume / breakCoefficient
			LReal hardnessCoefficient; //collision force module due for the shatter
			TrianglesList simplifyMesh; //simplified mesh for bullet physics

			void calculateVolumeMassInertia();
	};


	//Dynamic Object
	class DynamicObject {

		public:
			unsigned int idObj;
			unsigned int idGroup; //The objects that share the same idGroup have combined restitution = 0, is in DynamicObject for simplicity and because it is related to dynamic behavior
			unsigned int shatterLevel; //Number of recursive shatter from an object come from
			LVector3 position;
			LReal rotationAngle; //Radiant angle
			LVector3 rotationAxis;
			LVector3 scale; //Constant 1.0 TODO:not used
			LVector3 linearVelocity;
			LVector3 angularVelocity;
			bool isDynamic; //it is here because it is related to dynamic behavior

			//Not serialized variables:
			StaticObject staticObject; //staticObject copy with meshData clear for memory performance if not use_memory_for_meshes
			//Status that describes if the object is:
			//0 = the object is not still added to the world: this is the initial default state
			//1 = the object is added to the world and it is part of the simulation
			//2 = the object is removed from the world
			unsigned int statusInWorld;

			//note: in bullet seems that a (0,0,0) inertia object is static or a 0 mass object
	};

	typedef std::vector<DynamicObject> DynamicObjectVector;
	typedef boost::ptr_vector<DynamicObject> DynamicObjectPtrVector;

	//Collision Point
	class CollisionPoint {

		public:
			unsigned int idObj1;
			unsigned int idObj2;
			LVector3 contactPoint;
			LReal forceModule;
			LVector3 collisionDirection;

	};

	typedef std::vector<CollisionPoint> CollisionPointVector;


	//For the maximum Collision mapping algorithm
	typedef std::pair<unsigned int, unsigned int> PairUnsignedInt;
	typedef boost::unordered_map<PairUnsignedInt, CollisionPoint> MapOrderedPairCollisionPoint;
	typedef std::pair<PairUnsignedInt, CollisionPoint> PairCollisionPoint;
	typedef boost::unordered_map<unsigned int, CollisionPoint> MapObjectIdCollisionPoint;
	typedef std::pair<unsigned int, CollisionPoint> PairObjectIdCollisionPoint;
	void getMaxCollisionPoint(CollisionPointVector& collisionPointVector,
			MapOrderedPairCollisionPoint& maxMapCollisionPointPair, MapObjectIdCollisionPoint& maxMapCollisionPoint);

	template<class M>
	CollisionPointVector getCollisionPointVectorFromMap(M & unordered_map)
	{
		CollisionPointVector result;
		typename M::iterator collisionPointMapIter;
		for(collisionPointMapIter = unordered_map.begin();
				collisionPointMapIter != unordered_map.end();
				++collisionPointMapIter)
		{
			CollisionPoint collisionPoint = collisionPointMapIter->second;
			result.push_back(collisionPoint);
		}

		return result;
	}


	//Animation Frame
	class Frame {

		public:
			unsigned int idFrame;
			LReal timePosition;

			DynamicObjectVector dynamicObjects;
			CollisionPointVector collisionPoints;
			unsigned int lastCreatedIdObj;

	};


	//Animation Scene
	class Scene {

		public:
			LReal stepsPerSecond;


	};


	//Camera
	class Camera {

		public:
			unsigned int idFrame;
			LVector3 eyePosition;
			LVector3 lookAt;
			//LVector3 upVector; //TODO: not used
			bool keyframe;

			Camera() {
				keyframe = false;
			}

	};


	//load and save functions StaticObject
	void saveStaticObject(StaticObject &staticObject, const char * filename);
	void saveStaticObject(StaticObject &staticObject);
	StaticObject loadStaticObject(const char * filename);
	StaticObject loadStaticObject(int idObj);

	//load and save functions Frame
	void saveFrame(Frame &frame, const char * filename);
	void saveFrame(Frame &frame);
	void savePFrame(Frame &frame);
	Frame loadFrame(const char * filename);
	Frame loadFrame(int idFrame);
	Frame loadPFrame(int idFrame);

	//load and save functions Camera, and camera path building
	void saveCamera(Camera &camera, const char * filename);
	void saveCamera(Camera &camera);
	Camera loadCamera(const char * filename);
	Camera loadCamera(int idFrame);
	void computeCubicSplineCameraPath();

	class LarmorException: public std::exception
	{
		protected:
			 std::string message;

		public:
			 LarmorException(std::string m): message(m) {}
			 ~LarmorException() throw() {}
			 virtual const char* what() const throw()
			 {
				 return message.c_str();
			 }
	};


	const std::string currentDateTime();

	void createNotExistsDir(const char * dirname);

	inline btVector3 getBtFromLVector3(LVector3 &lv)
	{
		return btVector3(lv.x, lv.y, lv.z);
	}

	inline LVector3 getLFromBtVector3(btVector3 &btv)
	{
		return LVector3(btv.getX(), btv.getY(), btv.getZ());
	}

} //namespace LarmorPhysx

//Boost Serialization template for LVector3
namespace boost {
	namespace serialization {

		//LVector3 save: potrebbe anche essere utilizzato solo serialize come e' fatto per StaticObject
		template<class Archive>
		void save(Archive & ar, const LarmorPhysx::LVector3 & t, unsigned int version)
		{
			ar & t.x;
			ar & t.y;
			ar & t.z;
		}

		//LVector3 load
		template<class Archive>
		void load(Archive & ar, LarmorPhysx::LVector3 & t, unsigned int version)
		{
			LReal x, y, z;
			ar & x;
			ar & y;
			ar & z;
			LarmorPhysx::LVector3 newT(x, y, z);
			t = newT;
		}

		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::LVector3 & t, const unsigned int file_version)
		{
			split_free(ar, t, file_version);
		}
		//BOOST_SERIALIZATION_SPLIT_FREE(LVector3)


		//StaticObject
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::StaticObject & t, const unsigned int file_version)
		{
			ar & t.idObj;
			ar & t.idParentObj;
			ar & t.meshData;
			ar & t.volume;
			ar & t.mass;
			ar & t.density;
			ar & t.inertia;
			ar & t.friction;
			ar & t.restitution;
			ar & t.isConvex;
			//ar & t.idMaterial;
			ar & t.breakCoefficient;
			ar & t.hardnessCoefficient;
			ar & t.simplifyMesh;
		}

		//DynamicObject
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::DynamicObject & t, const unsigned int file_version)
		{
			ar & t.idObj;
			ar & t.idGroup;
			ar & t.shatterLevel;
			ar & t.position;
			ar & t.rotationAngle;
			ar & t.rotationAxis;
			ar & t.scale;
			ar & t.linearVelocity;
			ar & t.angularVelocity;
			ar & t.isDynamic;
		}

		//CollisionPoint
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::CollisionPoint & t, const unsigned int file_version)
		{
			ar & t.idObj1;
			ar & t.idObj2;
			ar & t.contactPoint;
			ar & t.forceModule;
			ar & t.collisionDirection;
		}

		//Frame
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::Frame & t, const unsigned int file_version)
		{
			ar & t.idFrame;
			ar & t.timePosition;
			ar & t.dynamicObjects;
			ar & t.collisionPoints;
			//ar & BOOST_SERIALIZATION_NVP(t.dynamicObjects); ???
			//ar & BOOST_SERIALIZATION_NVP(t.collisionPoints); ???
			ar & t.lastCreatedIdObj;
		}

		//Scene
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::Scene & t, const unsigned int file_version)
		{
			ar & t.stepsPerSecond;
		}

		//Camera
		template<class Archive>
		inline void serialize(Archive & ar, LarmorPhysx::Camera & t, const unsigned int file_version)
		{
			ar & t.idFrame;
			ar & t.eyePosition;
			ar & t.lookAt;
			//ar & t.upVector; //TODO: not used
			ar & t.keyframe;
		}


	} // namespace serialization
} // namespace boost



#endif /* CORE_CLASSES_H_ */
