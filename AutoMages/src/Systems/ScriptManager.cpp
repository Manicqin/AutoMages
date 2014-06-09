/*
 * ScriptManager.cpp
 *
 *  Created on: Jun 29, 2012mWorkerThread
 *      Author: manicqinmWorkerThread
 */
#include "LoggingSystem.h"
#include "ScriptManager.h"

#include "../EntityComponents/AllMessages.h"
#include "../EntityComponents/Entity.h"

#include "PythonBindings.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

void parseError()
{
	using namespace boost::python;
	PyObject *exc,*val,*tb;
	PyErr_Fetch(&exc,&val,&tb);
	PyErr_NormalizeException(&exc,&val,&tb);
	handle<> hexc(exc);
	handle<> hval(allow_null(val));
	handle<> htb(allow_null(tb));

	object traceback(import("traceback"));
	object format_exception(traceback.attr("format_exception"));
	object formatted_list(format_exception(hexc,hval,htb));
	object formatted(str("").join(formatted_list));
	std::string outMsg = extract<std::string>(formatted);

	LOG(_console = true,_newLine = false) << outMsg;
}



namespace Systems {
MsgSwitch::caseOperationRet callScriptFunc(std::string const& functionName , std::size_t const& entityId , boost::shared_ptr<Messages> val)
{
	auto msg = boost::make_shared<ScriptCallFunctionMsg>(functionName);

	msg->setEntity(entityId);
	msg->setArg(val);

	Systems::sendMsg(msg);

	return	MsgSwitch::caseOperationRet();
}
MsgSwitch::caseOperationRet wrapFunc(std::string const& functionName , std::size_t const& entityId , boost::shared_ptr<Messages> val) {
	using namespace boost;

	auto tmpCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::ScriptComponent>();
	auto tmpCmp =
			EntityComponent::ComponentRegistry<
			EntityComponent::ScriptComponent>::getSingletonPtr()->getComponent(
					tmpCmpId);
	auto tmpObj = tmpCmp->getData();

	try{
		python::call_method<void>(tmpObj,functionName.c_str(),python::ptr(val.get()));
	}
	catch(python::error_already_set &e)
	{
		parseError();
	}

	return msgSwitch::returnOk;
}
MsgSwitch::caseOperationRet wrapFuncNoParam(std::string const& functionName , std::size_t const& entityId) {
	using namespace boost;

	auto tmpCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::ScriptComponent>();
	auto tmpCmp =
			EntityComponent::ComponentRegistry<
			EntityComponent::ScriptComponent>::getSingletonPtr()->getComponent(
					tmpCmpId);
	auto tmpObj = tmpCmp->getData();

	static mutex	mutex;
	lock_guard<boost::mutex> lock(mutex);
	try{
		python::call_method<void>(tmpObj,functionName.c_str());
	}
	catch(python::error_already_set &e)
	{
		parseError();
	}
	return msgSwitch::returnOk;
}
ScriptSystem::ScriptSystem() :
		EnumedSystem<ScriptSystem>("ScriptSystem") {

	MessagesRegistry::addSubscriber(ScriptExecuteLine::getItemTypeId(),
			ScriptSystem::getItemTypeId());
	MessagesRegistry::addSubscriber(ScriptExecuteFile::getItemTypeId(),
			ScriptSystem::getItemTypeId());
	MessagesRegistry::addSubscriber(CreateScriptComponentMsg::getItemTypeId(),
				ScriptSystem::getItemTypeId());
	MessagesRegistry::addSubscriber(ScriptCallFunctionMsg::getItemTypeId(),
				ScriptSystem::getItemTypeId());

}

ScriptSystem::~ScriptSystem() {
}
msgSwitch ScriptSystem::execute(const MsgSwitch::caseOperationArg incomingMsg,
		ChannelHandler outChannel) {
	std::string retVal;
	if (ScriptExecuteLine::getItemTypeId() == incomingMsg->getMessageType()) {
		auto msg = CAST_MSG_TO_TYPE(ScriptExecuteLine,incomingMsg);
		if(msg)
            retVal = mPythonInterperter->executeLine(msg->getData(),outChannel);

	}
	else if(ScriptExecuteFile::getItemTypeId() == incomingMsg->getMessageType())
	{
		auto msg = CAST_MSG_TO_TYPE(ScriptExecuteFile,incomingMsg);
		if(msg)
            retVal = mPythonInterperter->executeFile(msg->getData(),outChannel);

	}
	else if(ScriptCallFunctionMsg::getItemTypeId() == incomingMsg->getMessageType())
	{
		auto msg = CAST_MSG_TO_TYPE(ScriptCallFunctionMsg,incomingMsg);
		if(msg)
		{
			auto functionName = msg->getData();
			auto entId = msg->getEntity();
			auto arg = msg->getArg();
            wrapFunc(functionName,entId,arg);
		}
	}
	else if(ChanneledMessages::getItemTypeId() == incomingMsg->getMessageType())
	{
		auto msg = CAST_MSG_TO_TYPE(ChanneledMessages,incomingMsg);
		if(msg)
            ScriptSystem::execute(msg->getWrapped(),msg->getBackChannel());

	}
	else
    {
        auto subList = mSubscribtionMap.equal_range(incomingMsg->getMessageType());
        std::for_each(subList.first,subList.second,[](std::pair<std::size_t,std::size_t>){
            
        });
        SystemBase::execute(incomingMsg,outChannel);
    }
	if(!retVal.empty())
		LOG(_console = true,_newLine = false) << retVal;

	return msgSwitch::returnOk;
}
//void ScriptSystem::operator ()(std::shared_ptr<Messages>	 const& incomingMsg,
//								ChannelHandler outChannel) {
//
//	MsgSwitch	msgSwitch;
//
//	msgSwitch.case_(ScriptExecuteLine::getItemTypeId(),[&](MsgSwitch::caseOperationArg const&  incomingMsg)
//			{
//				auto msg = std::dynamic_pointer_cast<ScriptExecuteLine>(incomingMsg);
//				if(msg)
//					mPythonInterperter->executeLine(msg->getData(),outChannel);
//				return MsgSwitch::caseOperationRet();
//			})
//			.case_(ScriptExecuteFile::getItemTypeId(),[&](MsgSwitch::caseOperationArg const& incomingMsg)
//			{
//				auto msg = std::dynamic_pointer_cast<ScriptExecuteFile>(incomingMsg);
//				if(msg)
//					mPythonInterperter->executeFile(msg->getData(),outChannel);
//				return MsgSwitch::caseOperationRet();
//			}
//		)(incomingMsg);
//
//	if(getChannelOut().isChannelOpen()) getChannelOut().addMsg(incomingMsg);
//}

DEFINE_CASE_IMP(Systems::ScriptSystem::startSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::ScriptSystem::createComponent)
{

	auto msg = CAST_MSG_TO_TYPE(CreateComponentMsg,val);
	auto ent = GET_ENTITY(msg->getEntityId());
	auto data = msg->getData();

	auto logger = LOG(_console = false);


	if(!ent)
	{
		LOG(_console = true) << boost::format("ScriptSystem::createComponent CANNOT FIND ENTITY!!! %d") % msg->getEntityId();
	}
	else
	{

		if(data->first_attribute("FileName"))
		{
			using namespace Systems;
			namespace fs = boost::filesystem;
			std::string strClassName = "SystemWrapper";

			auto oFileName = boost::lexical_cast<std::string>(data->first_attribute("FileName")->value());
			if(data->first_attribute("Class"))
				strClassName = boost::lexical_cast<std::string>(data->first_attribute("Class")->value());

            fs::path full_path;
            if(mScriptPath.empty())
            {
                full_path =  fs::initial_path<fs::path>();
                full_path/= "Scripts/macros";
            }
            else
            {
                full_path = mScriptPath;
            }
            fs::path macro_path = full_path / (oFileName);
			PyObject * pClass = nullptr;
			PyObject* pyEntId = PyInt_FromSize_t(msg->getEntityId());

			EntityComponent::ScriptComponent	cmpScript;
			auto cacheIter = mCache.find(macro_path.c_str());
			std::size_t newCmpId = 0;
			if (cacheIter != std::end(mCache))
			{
				pClass = cacheIter->second;
			}
			else
			{
				logger << boost::format("ScriptSystem::createComponent Loading script %s") % macro_path.c_str();
				//auto pObject = mPythonInterperter->createScriptObject(macro_path.c_str());

				std::string importPath = std::string(R"(sys.path.append(")") + full_path.c_str() + R"("))";
				std::vector<std::string>	out;
				boost::algorithm::split(out,oFileName,boost::is_any_of("."));
				PyObject*	pInstance = nullptr;
				PyRun_SimpleString("import sys");
				PyRun_SimpleString("import SystemsExports");
				PyRun_SimpleString("import MessagesExports");

				PyRun_SimpleString(importPath.c_str());

				auto pName = PyString_FromString(out[0].c_str());
				auto pModule = PyImport_Import(pName);
				PyErr_Print();
				auto pDict = PyModule_GetDict(pModule);
				pClass = PyDict_GetItemString(pDict,strClassName.c_str());

				mCache.insert(std::make_pair(macro_path.c_str(),pClass));

				Py_DecRef(pModule);
				Py_DecRef(pName);

			}

			PyObject* pInstance = nullptr;
			if (PyCallable_Check(pClass))
			{
				pInstance = PyObject_CallObject(pClass, nullptr);
			} else
			{
				PyErr_Print();
			}



			cmpScript.setData(pInstance);

			newCmpId = EntityComponent::ComponentRegistry<EntityComponent::ScriptComponent>::getSingletonPtr()->addComponent(cmpScript);

			ent->addComponent<EntityComponent::ScriptComponent>(newCmpId);
		}


	}

	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::ScriptSystem::initSystem)
{
	mPythonInterperter = new PythonBindings();

    auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);
    auto initData = msg->getData()->first_node("ScriptSystem");

    if(initData && initData->first_attribute("Path"))
    {
        mScriptPath = initData->first_attribute("Path")->value();

        LOG(_console = true) << boost::format("ScriptSystem::initSystem path nitialise: %s") % mScriptPath;
    }



	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::ScriptSystem::shutDownSystem)
{
	return MsgSwitch::caseOperationRet();
}

MsgSwitch::caseOperationArg	createMacroMsg(std::string strFileName)
{
	using namespace Systems;
	namespace fs = boost::filesystem;
	fs::path full_path( fs::initial_path<fs::path>() );
	full_path/= "Scripts/macros";
	fs::path macro_path = full_path / (strFileName);


	if(fs::exists(macro_path))
	{
		return MAKE_SHARED_MSG_TYPE<ScriptExecuteFile>(macro_path.c_str());
	}

	return MsgSwitch::caseOperationArg();

}

//template <>
//	MsgSwitch::caseOperationRet wrapFunc<Messages>(std::string const& functionName , std::size_t const& entityId , MsgSwitch::caseOperationArg val) {
//	using namespace boost;
//
//			auto tmpCmpId = GET_ENTITY(entityId)->getComponent<EntityComponent::ScriptComponent>();
//	auto tmpCmp =
//			EntityComponent::ComponentRegistry<
//					EntityComponent::ScriptComponent>::getSingletonPtr()->getComponent(
//					tmpCmpId);
//	auto tmpObj = tmpCmp->getData();
//
//	//Not sure if the best solution
//	//this should be handled using the python acquire lock functions
//	//But I couldn't make it work :(
//	static mutex	mutex;
//	lock_guard<boost::mutex> lock(mutex);
//	python::call_method<void,Messages*>(tmpObj,functionName.c_str(),val.get());
//	return msgSwitch::returnOk;
//}

}


namespace EntityComponent {
template<>
ComponentRegistry<ScriptComponent>* Ogre::Singleton<
	ComponentRegistry<ScriptComponent>>::msSingleton = nullptr;

//ScriptComponent buildScriptComponent(std::string strFileName) {
//
//}
}
