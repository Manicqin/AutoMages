/*
 * OgreMessages.h
 *
 *  Created on: Sep 23, 2012
 *      Author: manicqin
 */

#ifndef OGREMESSAGES_H_
#define OGREMESSAGES_H_
#include "Messages.h"

#include <Ogre.h>
#include "../Utilities/rapidxml/rapidxml.hpp"

MessageDeclare2(ObjectPosChangedMsg,Ogre::Quaternion,Orientation,Ogre::Vector3,Position);
//struct ObjectPosChangedMsg : public EnumMessages<ObjectPosChangedMsg>
//{
//public:
//	ObjectPosChangedMsg():EnumMessages<ObjectPosChangedMsg>("ObjectPosChangedMsg")
//	{}
//
//	~ObjectPosChangedMsg()
//	{}
//	Ogre::Quaternion  getOrientation() const { return mOrientation; }
//	void setOrientation(Ogre::Quaternion	const &newVal) { mOrientation = newVal; }
//
//	Ogre::Vector3 getPosition() const { return mPosition; }
//	void setPosition(Ogre::Vector3	const newVal) { mPosition = newVal; }
//private:
//	Ogre::Quaternion	mOrientation;
//	Ogre::Vector3	mPosition;
//};

DataMessageDeclareBegin(CreateCameraComponentMsg,std::shared_ptr<rapidxml::xml_node<>>)
DataMessageDeclareEnd2(std::size_t,EntityId)

//DataMessageDeclare2(CreateCameraComponentMsg,std::shared_ptr<rapidxml::xml_document<>>,std::size_t,EntityId);

DataMessageDeclare(TickMsg,Ogre::FrameEvent);
#endif /* OGREMESSAGES_H_ */
