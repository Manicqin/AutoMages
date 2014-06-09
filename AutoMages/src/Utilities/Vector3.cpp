/*
 * Vector3.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: manicqin
 */

#include "Vector3.h"
#include <OgreVector3.h>
#include <bullet/LinearMath/btVector3.h>
namespace Standard {

Vector3::Vector3(btVector3 const& newVal):mX(newVal.m_floats[0]),mY(newVal.m_floats[1]),mZ(newVal.m_floats[2])
{

}


Vector3::Vector3(Ogre::Vector3 const& newVal):mX(newVal.x),mY(newVal.y),mZ(newVal.z)
{

}

} /* namespace Standard */
