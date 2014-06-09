/*
 * PeriodicSystem.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: manicqin
 */

#include "PeriodicSystem.h"
#include "../EntityComponents/AllMessages.h"
namespace Systems
{
PeriodicSystem::PeriodicSystem():EnumedSystem <PeriodicSystem>("PeriodicSystem") {
	// TODO Auto-generated constructor stub

    getOperations().CASE_FUNC(TickMsg,PeriodicSystem::periodicalFunction);
}

PeriodicSystem::~PeriodicSystem() {
	// TODO Auto-generated destructor stub
}

DEFINE_CASE_IMP(PeriodicSystem::periodicalFunction)
{

	NotevMsg	B;
	B.printMsg("Sending");
	Systems::sendMsg(B.createSharedPtr());

	return MsgSwitch::caseOperationRet();
}
} /* namespace Systems */
