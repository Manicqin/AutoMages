/*
 * PhysicsSystem.h
 *
 *  Created on: May 11, 2012
 *      Author: manicqin
 */

#ifndef PHYSICSSYSTEM_H_
#define PHYSICSSYSTEM_H_


#include <bullet/btBulletDynamicsCommon.h>
#include <boost/property_tree/ptree_fwd.hpp>
//#include "../EntityComponents/PhysicsComponent.h"
#include "SystemBase.h"
#include "../EntityComponents/PhysicsMessages.h"

#include "../Utilities/Vector3.h"
namespace Systems {

class PhysicsSystem :  public EnumedSystem <PhysicsSystem>{
public:
	PhysicsSystem();

	virtual ~PhysicsSystem();

	bool addCollisionObject(btCollisionObject*	newRigidBody);
	void addRigidObject(btRigidBody* newRigidBody);

	void	run(Ogre::FrameEvent const& evt);

    virtual	DEFINE_CASE_PROTO(periodicalFunction);

	bool isLoopSimulation() const
	{
		return mLoopSimulation;
	}

	void setLoopSimulation(bool loopSimulation)
	{
		mLoopSimulation = loopSimulation;
	}

	const Standard::Vector3	getForceForEntity(std::size_t const& entityId);
	const Standard::Vector3	getLinearVelocityForEntity(std::size_t const& entityId);

private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;

	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btRigidBody* groundRigidBody;

	btCollisionShape* groundShape;
	btDefaultMotionState* groundMotionState;

	btVector3	mGravity;

	boost::thread	mPhysSysRoutine;	//Need to change so there will only one instance of the physRoutine
	btScalar mTimeStep;
	int mMaxSubSteps;
	btScalar mPrecision;
	static std::map<int,btCollisionShape*>	mFallShapeMap;

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);
	//MsgSwitch::caseOperationRet PositionChanged(MsgSwitch::caseOperationArg const& val);

	bool	mLoopSimulation;
	bool	mDebug;
	btIDebugDraw*	mDebugDrawer;

	//boost::mutex	mutex;
};

} /* namespace Systems */
#endif /* PHYSICSSYSTEM_H_ */
