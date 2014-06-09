/*
 * LocomotionSystem.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: manicqin
 */

#include "LocomotionSystem.h"
#include "../EntityComponents/SystemsMessages.h"
#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include <functional>
namespace Systems {

LocomotionSystem::LocomotionSystem():EnumedSystem <LocomotionSystem>("LocomotionSystem") {
	// TODO Auto-generated constructor stub

}

LocomotionSystem::~LocomotionSystem() {
	// TODO Auto-generated destructor stub
}


DEFINE_CASE_IMP(LocomotionSystem::createComponent)
{
	auto msg = dynamic_cast<CreateComponentMsg *>(val.get());

	/*
	 * 	LocomotionComponent
	 *
	 *
	 *
	 *	EntityWorldComponent
	 *
	 */

	auto lcmtnCmpId = GET_ENTITY(msg->getEntityId())->getComponent(EntityComponent::LocomotionComponent::getItemTypeId());

	if(lcmtnCmpId)	//There's allready a component
		auto lcmtnCmp = GET_COMP_REG(EntityComponent::LocomotionComponent).getComponent(lcmtnCmpId);


	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(LocomotionSystem::initSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(LocomotionSystem::shutDownSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(LocomotionSystem::startSystem)
{
	return	MsgSwitch::caseOperationRet();
}

}
 /* namespace EntityComponent */
