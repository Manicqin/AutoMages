/*
 * GraphicsMessages.h
 *
 *  Created on: Oct 27, 2012
 *      Author: manicqin
 */

#ifndef GRAPHICSMESSAGES_H_
#define GRAPHICSMESSAGES_H_

#include "Messages.h"

EmptyMessageDeclare(GraphicsRun);
DataMessageDeclare2(StartAnimationMsg,std::string,std::size_t,EntityID);

//DataMessageDeclare2(MoveCameraMsg,Ogre::Vector3,std::string,CameraID);

class MoveCameraMsg : public DataMsg<Ogre::Vector3, MoveCameraMsg>
{
public:
	MoveCameraMsg(Ogre::Vector3 const& data):DataMsg(data,"MoveCameraMsg") {}
	MoveCameraMsg(Ogre::Real const& x , Ogre::Real const& y , Ogre::Real const& z ):MoveCameraMsg(Ogre::Vector3(x,y,z)) {}

	std::string getCameraID() const { return mCameraID;}
	void setCameraID(std::string const& value){ mCameraID = value;}
	virtual ~MoveCameraMsg() {}

private:
	std::string mCameraID;
};


class CastRayMsg : public DataMsg<Ogre::Vector2, CastRayMsg>
{
public:
	CastRayMsg(Ogre::Vector2 const& data):DataMsg(data,"CastRayMsg") {}
	CastRayMsg(Ogre::Real const& x , Ogre::Real const& y ):CastRayMsg(Ogre::Vector2(x,y)) {}

	virtual ~CastRayMsg() {}
};


#endif /* GRAPHICSMESSAGES_H_ */
