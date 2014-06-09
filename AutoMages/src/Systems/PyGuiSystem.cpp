/*
 * PyGuiSystem.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: manicqin
 */

#include "PyGuiSystem.h"
#include "LoggingSystem.h"
#include "../EntityComponents/ScriptMessages.h"


#include <boost/filesystem.hpp>

namespace Systems {

PyGuiSystem::PyGuiSystem() {
	// TODO Auto-generated constructor stub

}

PyGuiSystem::~PyGuiSystem() {
	// TODO Auto-generated destructor stub
}


DEFINE_CASE_IMP(PyGuiSystem::startSystem)
{

	LogMsg().Log("CEGuiSystem::startSystem");

	auto path = boost::filesystem::initial_path() / R"(/Scripts/macros/Gui.py)";
	Systems::sendMsg(MAKE_SHARED_MSG_TYPE<ScriptExecuteFile>(path.native()));

	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PyGuiSystem::createComponent)
{


	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PyGuiSystem::initSystem)
{

	//auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);


	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(PyGuiSystem::shutDownSystem)
{

	return MsgSwitch::caseOperationRet();
}

} /* namespace Systems */
