/*
 * PhysicsMessages.h
 *
 *  Created on: Sep 28, 2012
 *      Author: manicqin
 */

#ifndef PHYSICSMESSAGES_H_
#define PHYSICSMESSAGES_H_

#include "Messages.h"
#include <btBulletDynamicsCommon.h>

#include "../Utilities/Vector3.h"


DataMessageDeclare(ChangeTimeStepMsg,btScalar);
DataMessageDeclare(ChangeGravityMsg,btVector3);
DataMessageDeclare(ChangeMaxSubStepMsg,int);

DataMessageDeclare(VectorMsg,Standard::Vector3);

DataMessageDeclare2(ApplyForceMsg,btVector3,std::size_t,EntityId);
DataMessageDeclare2(ApplyImpulseMsg,btVector3,std::size_t,EntityId);
DataMessageDeclare2(ApplyTorqueMsg,btVector3,std::size_t,EntityId);
DataMessageDeclare2(ApplyTorqueImpulseMsg,btVector3,std::size_t,EntityId);
DataMessageDeclare2(ApplyLinearVelocityMsg,btVector3,std::size_t,EntityId);

DataMessageDeclare(ClearForces,std::size_t);
MessageDeclare3(PositionChangedMsg,std::size_t,EntityId,btVector3,Position,btQuaternion,Orientation);

MessageDeclare2(CollisionMsg,std::size_t,EntityIdA,std::size_t,EntityIdB);

#endif /* PHYSICSMESSAGES_H_ */
