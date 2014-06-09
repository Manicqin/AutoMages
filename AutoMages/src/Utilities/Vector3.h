/*
 * Vector3.h
 *
 *  Created on: Nov 10, 2013
 *      Author: manicqin
 */

#ifndef VECTOR3_H_
#define VECTOR3_H_

namespace Ogre {
class Vector3;
};

class btVector3;


namespace Standard {



class Vector3
{
public:
	Vector3():mX(0),mY(0),mZ(0){}
	Vector3(float x,float y,float z):mX(x),mY(y),mZ(z){}

	Vector3(btVector3 const& newVal);
	Vector3(Ogre::Vector3 const& newVal);

	const float	getX() const { return mX; }
	void setX(float const& x) { mX = x; }

	const float	getY() const { return mY; }
	void setY(float const& y) { mY = y; }

	const float	getZ() const { return mZ; }
	void setZ(float const& z) { mZ = z; }

private:
	float mX,mY,mZ;
};

} /* namespace Standard */
#endif /* VECTOR3_H_ */
