/*
 * ScriptManager.h
 *
 *  Created on: Jun 29, 2012
 *      Author: manicqin
 *//*
 * SystemsManager.h
 *
 *  Created on: Sep 22, 2012
 *      Author: manicqin
 */

#ifndef SCRIPTSYSTEM_H_
#define SCRIPTSYSTEM_H_

#include <boost/thread.hpp>
#include "SystemBase.h"
#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/ScriptComponent.h"

#include <boost/python/object.hpp>
#include <boost/python/call_method.hpp>


void parseError();


class PythonBindings;

namespace EntityComponent{

ScriptComponent	buildScriptComponent(std::string strFileName);	//builds a script component object from a python code
}

namespace Systems {


//.case_<ControllerMousePressedMsg>(std::bind(wrapFunc<ControllerMousePressedMsg>, "onControllerMousePressedMsg" , getSystemEntityId() , std::placeholders::_1));

#define CASE_WRAP_SCRIPT_FUNC(msgType)	case_<msgType>(std::bind(callScriptFunc, "on" #msgType , getSystemEntityId() , std::placeholders::_1))
//#define CASE_WRAP_SCRIPT_FUNC2(msgType,funcName)	case_<msgType>(std::bind(wrapFunc<msgType>, #funcName , getSystemEntityId() , std::placeholders::_1))


MsgSwitch::caseOperationRet callScriptFunc(std::string const& functionName , std::size_t const& entityId , boost::shared_ptr<Messages> val);

//template <class T>
//	MsgSwitch::caseOperationRet wrapFunc(std::string const& functionName , std::size_t const& entityId , boost::shared_ptr<Messages> val) {
//	using namespace boost;
//
//			auto tmpCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::ScriptComponent>();
//	auto tmpCmp =
//			EntityComponent::ComponentRegistry<
//					EntityComponent::ScriptComponent>::getSingletonPtr()->getComponent(
//					tmpCmpId);
//	//LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"), "wrapFunc: " << tmpCmpId << " Ent: " << entityId);
//	auto tmpObj = tmpCmp->getData();
//
//	//Not sure if the best solution
//	//this should be handled using the python acquire lock functions
//	//But I couldn't make it work :(
//	//static mutex	mutex;
//	//lock_guard<boost::mutex> lock(mutex);
//	try{
//		python::call_method<void,T*>(tmpObj,functionName.c_str(),dynamic_cast<T*>(val.get()));
//	}
//	catch(python::error_already_set &e)
//	{
//		parseError();
//	}
//
//	return msgSwitch::returnOk;
//}

MsgSwitch::caseOperationRet wrapFunc(std::string const& functionName , std::size_t const& entityId , boost::shared_ptr<Messages> val);

MsgSwitch::caseOperationRet wrapFuncNoParam(std::string const& functionName , std::size_t const& entityId);

MsgSwitch::caseOperationArg	createMacroMsg(std::string strFileName);

class ScriptSystem : public EnumedSystem <ScriptSystem>{
public:
	ScriptSystem();
	virtual ~ScriptSystem();

	msgSwitch execute(const MsgSwitch::caseOperationArg incomingMsg,
			ChannelHandler outChannel);

private:
    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

	PythonBindings*	mPythonInterperter;
    std::string     mScriptPath;
	std::map<std::string,PyObject* >	mCache;

    std::multimap<std::size_t , std::size_t>    mSubscribtionMap;
};
}
/* namespace Systems */



#endif /* SYSTEMSMANAGER_H_ */

