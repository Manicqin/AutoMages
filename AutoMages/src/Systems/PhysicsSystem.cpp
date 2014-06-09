/*
 * PhysicsSystem.cpp
 *
 *  Created on: May 11, 2012
 *      Author: manicqin
 */

#include "PhysicsSystem.h"
#include "LoggingSystem.h"
#include "SystemsManager.h"
#include "OgreSystem.h"

#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/AllMessages.h"

#include "../Utilities/Physics/PhysicsUtils.h"
#include "../Utilities/Physics/DebugDrawer.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

#include <random>
#include <time.h>

namespace Systems {

std::map<int,btCollisionShape* >	PhysicsSystem::mFallShapeMap;

bool didCompMoved(boost::shared_ptr<EntityComponent::PhysicsComponent> const& physComp,
							boost::shared_ptr<EntityComponent::EntityWorldComponent> const&  wrldCmp,
								float precision)
{
	btTransform trans;
	physComp->getRigidBody()->getMotionState()->getWorldTransform(trans);
	Ogre::Vector3	physPos(trans.getOrigin().x(),trans.getOrigin().y(),trans.getOrigin().z());
	return wrldCmp->getPosition().distance(physPos) > precision;
};

bool didEntityMoved(std::size_t const& entityId,
								float precision)
{

	auto physCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::PhysicsComponent>();
	auto wrldCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::EntityWorldComponent>();



	return didCompMoved(GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId),
							GET_COMP_REG(EntityComponent::EntityWorldComponent).getComponent(wrldCmpId),
								precision);
};

PhysicsSystem::PhysicsSystem():EnumedSystem <PhysicsSystem>("PhysicsSystem"),
		mGravity(0,-10,0),
		mTimeStep(1 / 5.f),
		mMaxSubSteps(5),
		dynamicsWorld(nullptr),
		mPrecision(0.001),
		mDebug(false),
		mLoopSimulation(false)
		{

            getOperations().case_<ChangeTimeStepMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ChangeTimeStepMsg *>(val.get());
						mTimeStep = msg->getData();
						return MsgSwitch::caseOperationRet();
					})
            .case_<ApplyForceMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ApplyForceMsg *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getEntityId())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						physCmp->getRigidBody()->applyCentralForce(msg->getData());
						return MsgSwitch::caseOperationRet();
					})
            .case_<ApplyImpulseMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ApplyImpulseMsg *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getEntityId())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						physCmp->getRigidBody()->applyCentralImpulse(msg->getData());
						return MsgSwitch::caseOperationRet();
					})
            .case_<ApplyTorqueImpulseMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ApplyTorqueImpulseMsg *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getEntityId())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						physCmp->getRigidBody()->applyTorqueImpulse(msg->getData());
						return MsgSwitch::caseOperationRet();
					})
            .case_<ApplyTorqueMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ApplyImpulseMsg *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getEntityId())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						physCmp->getRigidBody()->applyTorque(msg->getData());
						return MsgSwitch::caseOperationRet();
					})
            .case_<ApplyLinearVelocityMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ApplyLinearVelocityMsg *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getEntityId())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						auto body = physCmp->getRigidBody();
						auto origin = body->getWorldTransform().getOrigin();
						auto destination = msg->getData();
						btVector3 linVel(destination[0]-origin[0],destination[1]-origin[1],destination[2]-origin[2]);
						linVel.normalize();
						linVel*=40.f;

						//body->getWorldTransform().setOrigin(camPos);
						body->getWorldTransform().setRotation(btQuaternion(0,0,0,1));
						body->setLinearVelocity(linVel);
						body->setAngularVelocity(btVector3(0,0,0));
						body->setCcdMotionThreshold(0.5);
						body->setCcdSweptSphereRadius(0.4f);

						return MsgSwitch::caseOperationRet();
					})
            .case_<ClearForces>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ClearForces *>(val.get());
						auto physCmpId = GET_ENTITY(msg->getData())->getComponent<EntityComponent::PhysicsComponent>();
						auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

						physCmp->getRigidBody()->clearForces();
						return MsgSwitch::caseOperationRet();
					})
            .case_<ChangeMaxSubStepMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ChangeMaxSubStepMsg *>(val.get());
						mMaxSubSteps = msg->getData();
						return MsgSwitch::caseOperationRet();
					})
            .case_<ChangeGravityMsg>([&](MsgSwitch::caseOperationArg  const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
					{
						auto msg = dynamic_cast<ChangeGravityMsg *>(val.get());
						dynamicsWorld->setGravity(msg->getData());
						return MsgSwitch::caseOperationRet();
					})
            .CASE_FUNC(TickMsg,PhysicsSystem::periodicalFunction);
			//.case_<ControllerKeyMsg>(std::bind(&PhysicsSystem::PositionChanged,this,std::placeholders::_1));;
			srand(time(NULL));
		}

PhysicsSystem::~PhysicsSystem() {
	//	std::for_each(getObjectList().begin(),getObjectList().end(),[&](AnimatedObject * obj)
	//			{
	//				this->dynamicsWorld->removeRigidBody(obj->getRigidBody());
	//			});
	//	dynamicsWorld->removeRigidBody(fallRigidBody);
	//delete fallRigidBody->getMotionState();
	//delete fallRigidBody;
	dynamicsWorld->removeRigidBody(groundRigidBody);

	std::cout << "Rigid bodies removed" << std::endl;
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	delete groundShape;

	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;

	std::cout << "Graphic Manger is destroyed" << std::endl;
}

bool PhysicsSystem::addCollisionObject(btCollisionObject*	newRigidBody){
	dynamicsWorld->addCollisionObject(newRigidBody);
	return true;
}


//PhysicsSystem* PhysicsSystem::getSingletonPtr(void)
//{
//	return msSingleton;
//}
//PhysicsSystem& PhysicsSystem::getSingleton(void)
//{
//	assert( msSingleton );  return ( *msSingleton );
//}



void PhysicsSystem::addRigidObject(btRigidBody* newRigidBody) {
	dynamicsWorld->addRigidBody(newRigidBody);
}

void PhysicsSystem::run(Ogre::FrameEvent const& evt) {
	try{
	{
		if(isLoopSimulation() && dynamicsWorld)
		{
			{
				//boost::lock_guard<boost::mutex> lock(mutex);
				dynamicsWorld->stepSimulation(evt.timeSinceLastFrame, 5);
			}
			if(mDebug)
			{
				dynamicsWorld->debugDrawWorld();
			}
//			LogMsg(0,0,false).Log("Stepping");
			btTransform trans;
			GET_COMP_REG(EntityComponent::PhysicsComponent).iterarteOver(
					[&](std::pair<std::size_t,boost::shared_ptr<EntityComponent::PhysicsComponent>> val)
					{
						btTransform trans;
						if(val.second->getRigidBody())
						{
							if(val.second->getRigidBody()->getMotionState())
							{
								val.second->getRigidBody()->getMotionState()->getWorldTransform(trans);

								auto entID = static_cast<std::size_t *>( val.second->getRigidBody()->getUserPointer());
								auto wrldCmpId = GET_ENTITY(*entID)->getComponent<EntityComponent::EntityWorldComponent>();
								auto wrldCmp = GET_COMP_REG(EntityComponent::EntityWorldComponent).getComponent(wrldCmpId);
								Ogre::Vector3	physPos(trans.getOrigin().x(),trans.getOrigin().y(),trans.getOrigin().z());

								if(wrldCmp && didCompMoved(val.second,wrldCmp,mPrecision))
								{
									LogMsg(0,0,false) << boost::format("Physics: %d: %f %f %f") % *entID % trans.getOrigin().x()%trans.getOrigin().y()%trans.getOrigin().z();
									wrldCmp->setPosition(physPos);

									Systems::sendMsg(boost::make_shared<PositionChangedMsg>(*entID,trans.getOrigin(),trans.getRotation()));
//									if(getChannelOut().isChannelOpen())
//										getChannelOut().addMsg(boost::make_shared<PositionChangedMsg>(*entID,trans.getOrigin(),trans.getRotation()));
								}

							}
						}
					}
			);

	//		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	//		//std::cout << "collisions amount: " << numManifolds << std::endl;
	//		for (auto iDx = 0;iDx < numManifolds;++iDx)
	//		{
	//			btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(iDx);
	//			btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
	//			btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
	//
	//			auto entA = static_cast<std::size_t *>( obA->getUserPointer());
	//			auto entB = static_cast<std::size_t *>( obB->getUserPointer());
	//
	//			if(entA && entB)
	//			{
	//				//if(didEntityMoved(*entA,0.01) && didEntityMoved(*entB,0.01))
	//				if(getChannelOut().isChannelOpen())
	//					getChannelOut().addMsg(std::shared_ptr<Messages>(new CollisionMsg(*entA,*entB)));
	//			}
	//			int numContacts = contactManifold->getNumContacts();
	//			for (int j=0;j<numContacts;j++)
	//			{
	//				btManifoldPoint& pt = contactManifold->getContactPoint(j);
	//
	//				if (pt.getDistance()<0.f)
	//				{
	//					const btVector3& ptA = pt.getPositionWorldOnA();
	//					const btVector3& ptB = pt.getPositionWorldOnB();
	//					const btVector3& normalOnB = pt.m_normalWorldOnB;
	//				}
	//			}
	//		}
		}
		else
		{
			if((rand()%10)==0)
				LogMsg(0,0,false).Log("dynamicsWorld not initialized");
		}
		//boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0/240));
	}
	}
	catch(...)
	{
		std::cout << "bla bla bla" << std::endl;
	}
}

DEFINE_CASE_IMP(PhysicsSystem::createComponent)
{
	auto msg = CAST_MSG_TO_TYPE(CreateComponentMsg,val);
	/*
	*	Component creator:
	*		*Do I send only the desired component data?
	*		*Do I send all of the entity data?
	*		*Can I send a bulk of components?
	*		*How do I send back the authorization?
	*
	*	I can think of a few ways to resolve:
	*		*each system gets only the specific component information
	*			A component request can be delivered with pointer to the entity so it will have the ability to
	*			register the component at the entity.
	*		*each system gets ALL the entity information and uses only what is needs but has knowledge
	*			of the other data the entity has (and then maybe use the entity name or entity information)
	*
	*	What data does the physics component holds?
	*	*Mass
	*	*btRigidBody
	*	*ShapeInformation
	*
	* 	PhysComp creation has to rely on RenderComp (for now) for data such as
	* 		position
	* 		orientation
	* 		scale
	* 		envelope
	*
	* 	Adding new component
	* 		EntityWorldComp
	*/

	//Need to find a way to shorten the next few lines!
	//These 2 lines are as followed
	//Get the EntityWorldComponent ID of the entity
	//Get the EntityWorldComponent component from the EntityWorldComponent registry

	auto	logger = LOG(_console = false);
	auto ent = GET_ENTITY(msg->getEntityId());

	std::size_t wrldCmpId = 0;
	if(!ent)
	{
		std::cout << "CANNOT FIND ENTITY!!! " << msg->getEntityId() << std::endl;
		BOOST_ASSERT_MSG(ent,"PhysicsSystem::createComponent");
	}
	else
		wrldCmpId = ent->getComponent(EntityComponent::EntityWorldComponent::getItemTypeId());

	logger << boost::format("PhysicsSystem::createComponent entity %d using EntityWorldComponent %d") % msg->getEntityId() % wrldCmpId;
	auto wrldCmp = GET_COMP_REG(EntityComponent::EntityWorldComponent).getComponentView(wrldCmpId);

	//Basically there is no reason for RenderComponent AND EntityWorldComponent because the
	boost::shared_ptr<EntityComponent::PhysicsComponent>	tmpPhysCmp(new EntityComponent::PhysicsComponent);

	auto cmpMass = msg->getData()->first_attribute("Mass");
	if(cmpMass)
		tmpPhysCmp->setMass(boost::lexical_cast<Ogre::Real>(cmpMass->value()));
	else
		tmpPhysCmp->setMass(0);

	//I had a weird problem, I wouldn't find "Envelope" but for some reason the not_found I got
	//back as a return value of find was different from the not_found I got from the function not_found
	//I changed the dataMsg structure to encapsulate a const T and now it seems to work
	//I'll keep this try and catch for now...
	try{
		if(!msg->getData()->first_node("Envelope"))
			tmpPhysCmp->setEnvelope(btVector3(0,0,0));
		else
			tmpPhysCmp->setEnvelope(readVectorXml<btVector3>(msg->getData()->first_node("Envelope")));
	}
	catch(...)
	{
		logger << boost::format("PhysicsSystem::createComponent entity %d Envelope problem") % msg->getEntityId();
	}

	btMotionState* fallMotionState;
	btCollisionShape* fallShape;
	btMatrix3x3	tmpMatrix;
	tmpMatrix.setEulerYPR(wrldCmp->getOrientation().x,wrldCmp->getOrientation().y,wrldCmp->getOrientation().z);
	//Each Collision shape should be hold only once, so I calculate a hash (naively) and only create a new
	//Collision shape if needed.

	auto rndrCmpId = ent->getComponent(EntityComponent::RenderComponent::getItemTypeId());

	Ogre::Vector3 HalfExtent;

	//If the current component has a graphical component
	if(GET_COMP_REG(EntityComponent::RenderComponent).getComponentView(rndrCmpId) &&
			GET_COMP_REG(EntityComponent::RenderComponent).getComponentView(rndrCmpId)->getSceneNode()->numAttachedObjects())
	{
		//LogMsg().Log("rndrCmpId");
		auto rndrCmp = GET_COMP_REG(EntityComponent::RenderComponent).getComponentView(rndrCmpId);
		auto obj = rndrCmp->getSceneNode()->getAttachedObject(0);
		HalfExtent = obj->getBoundingBox().getHalfSize();
		//LogMsg() << boost::format("HalfExtent %d") % HalfExtent;

//		fallMotionState = new MotionStateBltOgre(
//				btTransform(tmpMatrix,btVector3(wrldCmp->getPosition().x,wrldCmp->getPosition().y,wrldCmp->getPosition().z)),
//						rndrCmp->getSceneNode());

		fallMotionState = new btDefaultMotionState(btTransform(tmpMatrix,
						btVector3(wrldCmp->getPosition().x,
								wrldCmp->getPosition().y,
								wrldCmp->getPosition().z)));

	}
	else
	{
		logger << boost::format("PhysicsSystem::createComponent entity %d doesn't have RenderComponent so we need to use the Envelope field from physicsComponent... hope you supplied it") % msg->getEntityId();
		HalfExtent = Ogre::Vector3(tmpPhysCmp->getEnvelope().x(),tmpPhysCmp->getEnvelope().y(),tmpPhysCmp->getEnvelope().z());
		fallMotionState = new btDefaultMotionState(btTransform(tmpMatrix,
						btVector3(wrldCmp->getPosition().x,
								wrldCmp->getPosition().y,
								wrldCmp->getPosition().z)));
	}


	tmpPhysCmp->setObjectHash(HalfExtent.dotProduct(wrldCmp->getScale()));
	auto iter =	mFallShapeMap.find(tmpPhysCmp->getObjectHash());
	if(iter != mFallShapeMap.end())
	{
		fallShape = iter->second;
	}
	else
	{
		auto mulRes = HalfExtent * wrldCmp->getScale();
		fallShape = new btBoxShape(btVector3(mulRes.x,mulRes.y,mulRes.z));
		mFallShapeMap[tmpPhysCmp->getObjectHash()] = fallShape;
	}

	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(tmpPhysCmp->getMass(),fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(tmpPhysCmp->getMass(),fallMotionState,fallShape,fallInertia);
	tmpPhysCmp->setRigidBody(std::shared_ptr<btRigidBody>(new btRigidBody(fallRigidBodyCI)));

	auto userPntr = new std::size_t;
		*userPntr = msg->getEntityId();
		tmpPhysCmp->getRigidBody()->setUserPointer((void*)userPntr);

	{
			//boost::lock_guard<boost::mutex> lock(mutex);
				dynamicsWorld->addRigidBody(tmpPhysCmp->getRigidBody().get());
	}

	auto cmpGravity = msg->getData()->first_node("Gravity");
	if(cmpGravity)
		tmpPhysCmp->getRigidBody()->setGravity(readVectorXml<btVector3>(msg->getData()->first_node("Gravity")));

	auto physCmpId = GET_COMP_REG(EntityComponent::PhysicsComponent).addComponent(tmpPhysCmp);
	//LogMsg() << boost::format("PhysicsSystem::CreateComponent entity %d created PhysicsComponent %d") % msg->getEntityId() % physCmpId;
	GET_ENTITY(msg->getEntityId())->addComponent<EntityComponent::PhysicsComponent>(physCmpId);

	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PhysicsSystem::initSystem)
{

	auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);
	auto initData = msg->getData()->first_node("PhysicsSystem");
	//Physics init Data:
	//Gravity	: 	vector3	:	optional
	//TimeStep	:	Real	:	optional
	//MaxSubSteps:	Real	:	optional
	//groundplane:	vector3	:	optinal

	btVector3	groundPlaneNormal(0,1,0);
	if(initData && initData->first_node("Gravity"))
		mGravity = readVectorXml<btVector3>(initData->first_node("Gravity"));

	if(initData && initData->first_node("GroundPlaneNormal"))
		groundPlaneNormal = readVectorXml<btVector3>(initData->first_node("GroundPlaneNormal"));

	if(initData && initData->first_attribute("MaxSubSteps"))
	{
		mMaxSubSteps = boost::lexical_cast<int>(initData->first_attribute("MaxSubSteps")->value());
		mTimeStep = 1. / mMaxSubSteps;
	}

	if(initData && initData->first_attribute("TimeStep"))
		mTimeStep = boost::lexical_cast<int>(initData->first_attribute("TimeStep")->value());

	if(initData && initData->first_attribute("Precision"))
		mPrecision = boost::lexical_cast<btScalar>(initData->first_attribute("Precision")->value());

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);

	dynamicsWorld->setGravity(mGravity);

	groundShape = new btStaticPlaneShape(groundPlaneNormal,1);
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo	groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PhysicsSystem::shutDownSystem)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PhysicsSystem::startSystem)
{
//	mPhysSysRoutine = boost::thread(&PhysicsSystem::run,this);

	setLoopSimulation(true);
	if(mDebug)
	{
		mDebugDrawer = new OgreDebugDrawer( SystemsManager::getSingletonPtr()->getSystem<OgreSystem>()->getSceneMgr() );
	    mDebugDrawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe );
	    dynamicsWorld->setDebugDrawer( mDebugDrawer );

	}

	return	MsgSwitch::caseOperationRet();
}

//MsgSwitch::caseOperationRet PhysicsSystem::PositionChanged(
//		MsgSwitch::caseOperationArg const& val) {
//
//	auto trans = dynamicsWorld->getCollisionObjectArray().at(0)->getWorldTransform();
//	auto pos = trans.getOrigin();
//	pos.setX(pos.getX()+1000);
//	pos.setY(pos.getY()+1000);
//	trans.setOrigin(pos);
//	dynamicsWorld->getCollisionObjectArray().at(0)->setWorldTransform(trans);
//	return	MsgSwitch::caseOperationRet();
//
//}
DEFINE_CASE_IMP(PhysicsSystem::periodicalFunction)
{
	auto msg = CAST_MSG_TO_TYPE(TickMsg,val);
	run(msg->getData());
	return MsgSwitch::caseOperationRet();
}

	const Standard::Vector3 PhysicsSystem::getForceForEntity(const std::size_t& entityId) {
		auto physCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::PhysicsComponent>();
		auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

		return physCmp->getRigidBody()->getTotalForce();
	}

	const Standard::Vector3 PhysicsSystem::getLinearVelocityForEntity(const std::size_t& entityId) {
		auto physCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::PhysicsComponent>();
		auto physCmp = GET_COMP_REG(EntityComponent::PhysicsComponent).getComponent(physCmpId);

		return physCmp->getRigidBody()->getLinearVelocity();
	}
}




