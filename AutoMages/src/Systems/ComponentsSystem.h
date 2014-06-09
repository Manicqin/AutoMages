/*
 * ComponentsSystem.h
 *
 *  Created on: Oct 9, 2012
 *      Author: manicqin
 */

#ifndef COMPONENTSSYSTEM_H_
#define COMPONENTSSYSTEM_H_
#include "SystemBase.h"
#include "ScriptManager.h"
#include "LoggingSystem.h"

#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/ScriptComponent.h"

#define CASE_WRAP_SCRIPT_FUNC_FOR_ENT(msgType)	case_<msgType>(std::bind(wrapFuncEnt<msgType>, "on" #msgType , std::placeholders::_1))
//#define CASE_WRAP_SCRIPT_FUNC2(msgType,funcName)	case_<msgType>(std::bind(wrapFunc<msgType>, #funcName , std::placeholders::_1, std::placeholders::_2))

	template <class T>
	MsgSwitch::caseOperationRet wrapFuncEnt(std::string const& functionName ,MsgSwitch::caseOperationArg const& val) {
	using namespace boost;

	auto msg = CAST_MSG_TO_TYPE(T,val);

	auto tmpCmpId = GET_ENTITY(msg->getData())->getComponent(EntityComponent::ScriptComponent::getItemTypeId());
	if(!tmpCmpId)
	{
		LogMsg() << boost::format("No ScriptComponent for ent %d") % msg->getData();
	}
	else
	{
		auto tmpCmp =
				EntityComponent::ComponentRegistry<
						EntityComponent::ScriptComponent>::getSingletonPtr()->getComponent(
						tmpCmpId);
		//LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"), "wrapFunc: " << tmpCmpId << " Ent: " << entityId);
		auto tmpObj = tmpCmp->getData();

		//Not sure if the best solution
		//this should be handled using the python acquire lock functions
		//But I couldn't make it work :(
		static mutex	mutex;
		lock_guard<boost::mutex> lock(mutex);
		try{
			python::call_method<void>(tmpObj,functionName.c_str(),python::ptr(val.get()));
		}
		catch(python::error_already_set &e)
		{
			parseError();
		}
	}
	return msgSwitch::returnOk;
}



namespace Systems {

class ComponentsSystem  : public EnumedSystem <ComponentsSystem>{
public:
	ComponentsSystem();

	virtual ~ComponentsSystem();

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

	//I didn't had good names :(
	//InitEntity (for now) is the function that get's the <ControllerMousePressedMsg> and creates the XML for an entity
	//next it will be sent to createEntity to materialize the entity;
    DEFINE_CASE_PROTO(initEntity);
    DEFINE_CASE_PROTO(createEntity);
    DEFINE_CASE_PROTO(createScene);
    DEFINE_CASE_PROTO(getEntityList);

private:

	bool	mWaitOnComponentBuild;
	bool	mWaitOnEntityBuild;
	bool	mWaitOnSceneBuild;

	ChannelHandler	mWaitChannel;
};

} /* namespace Systems */
#endif /* COMPONENTSSYSTEM_H_ */
