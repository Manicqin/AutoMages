/*
 * SystemsExports.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: manicqin
 */
#ifndef SYSTEMSEXPORTS_H_
#define SYSTEMSEXPORTS_H_

#include "../Systems/SystemBase.h"
#include "../Systems/SystemsManager.h"
#include "../Systems/LoggingSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/ScriptManager.h"

#include "../Utilities/ConfigurationDirectory.h"
//#include "../EntityComponents/Component.h"

#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <python2.7/import.h>

void registerOperation(std::size_t systemId , std::string funcName , std::size_t msgId)
{
	auto sys = Systems::SystemsManager::getSingleton().getSystem(systemId);
	BOOST_ASSERT_MSG(sys,"No System!!");
	if(sys)
	{
		sys->getOperations()
            .case_(std::bind(Systems::callScriptFunc, funcName ,sys->getSystemEntityId() , std::placeholders::_1),msgId );
	}
	else
	{
		throw("registerOperation cannot register operation, cannot find the system by the id");
	}

}

std::size_t	getSystemIdByName(std::string const& systemName)
{
	return SystemsRegistry::getItemIdByName(systemName);
}

boost::shared_ptr<Systems::SystemBase>	getSystem(std::size_t const& sysId)
{
	return Systems::SystemsManager::getSingleton().getSystem(sysId);
}

Systems::ChannelHandler	getSystemChannelIn(std::size_t	const& systemId)
{
	auto system = Systems::SystemsManager::getSingleton().getSystem(systemId);
	BOOST_ASSERT_MSG(system,"No System!!");
	return system->getChannelIn();
}

boost::shared_ptr<Systems::PhysicsSystem> getPhysicsSystem()
{
	return Systems::SystemsManager::getSingleton().getSystem<Systems::PhysicsSystem>();
}

Systems::ChannelHandler	getSystemChannelOut(std::size_t	const& systemId)
{
	return Systems::SystemsManager::getSingleton().getSystem(systemId)->getChannelOut();
}

void setSystemChannelOut(std::size_t	const& systemId,Systems::ChannelHandler	channel)
{
	Systems::SystemsManager::getSingleton().getSystem(systemId)->setChannelOut(channel);
}

void sendMsgToSystem(std::size_t	systemId, MsgSwitch::caseOperationArg	newMsg)
{
	getSystemChannelIn(systemId).addMsg(newMsg);
}

void	LogFunc(std::string const& msg)
{
	LOG().Log_(msg);
}

class logWraper
{
public:
	static void write(std::string msg)
	{
		LogFunc(msg);
	}
};
Systems::ChannelHandler	createChannelHandler(std::string const& channelName)
{
	//asm volatile ("int3;");
	return Systems::ChannelHandler::createChannelHandler<Systems::ThreadSafeChannel>(channelName);
}

ConfigurationDirectory* getConfiguration()
{

    return ConfigurationDirectory::getSingletonPtr();
}

boost::shared_ptr<Messages> getMsgFromChannel(Systems::ChannelHandler inChannel)
{
    boost::shared_ptr<Messages> retVal = nullptr;
    if (inChannel.isChannelOpen() && inChannel.hasMsgs())
        inChannel.getMsg(retVal);

    return retVal;
}

BOOST_PYTHON_FUNCTION_OVERLOADS(waitOnChannel_Overloads,Systems::waitOnChannel,1,4);

BOOST_PYTHON_MODULE(SystemsExports)
{

	boost::python::def("getSystemIdByName",&getSystemIdByName);

	boost::python::class_<Systems::ChannelHandler>("ChannelHandler",boost::python::init<>())
						.def(boost::python::init<std::string const&>())
						.def("addMsg",&Systems::ChannelHandler::addMsg)
						.def("getMsg",&Systems::ChannelHandler::getMsg)
						.def("releaseChannel",&Systems::ChannelHandler::releaseChannel)
						.add_property("numOfInstances",&Systems::ChannelHandler::numOfInstances)
						.add_property("hasMsgs",&Systems::ChannelHandler::hasMsgs)
						.add_property("channelName",&Systems::ChannelHandler::getChannelName);

	boost::python::def("createChannelHandler",createChannelHandler);
	boost::python::def("waitOnChannel",&Systems::waitOnChannel,waitOnChannel_Overloads());

	boost::python::def("getSystemChannelIn",&getSystemChannelIn);
	boost::python::def("getSystemChannelOut",&getSystemChannelOut);
	boost::python::def("setSystemChannelOut",&setSystemChannelOut);

	boost::python::def("sendMsgToSystem",&sendMsgToSystem);
    boost::python::def("getMsgFromChannel",&getMsgFromChannel);
	boost::python::def("sendMsg",&Systems::sendMsg);

	boost::python::class_<logWraper>("logWraper")
					.def("write",&logWraper::write).staticmethod("write");

	boost::python::def("LogFunc",&LogFunc);
	boost::python::class_<LogMsg>("LogMsg",boost::python::init<>())
					.def("Log",&LogMsg::Log_);

	boost::python::class_<Standard::Vector3>("Vector3",boost::python::init<>())
		.add_property("x",&Standard::Vector3::getX,&Standard::Vector3::setX)
		.add_property("y",&Standard::Vector3::getY,&Standard::Vector3::setY)
		.add_property("z",&Standard::Vector3::getZ,&Standard::Vector3::setZ);
		//.def_pickle(Vector3_pickle_suite());

	boost::python::def("getPhysicsSystem",&getPhysicsSystem);

	boost::python::class_<Systems::PhysicsSystem,boost::shared_ptr<Systems::PhysicsSystem>,boost::noncopyable>("PhysicsSystem")
			.def("getForceForEntity",&Systems::PhysicsSystem::getForceForEntity)
			.def("getLinearVelocityForEntity",&Systems::PhysicsSystem::getLinearVelocityForEntity);

	boost::python::class_<Systems::SystemBase,
							boost::shared_ptr<Systems::SystemBase>,
								boost::noncopyable>("SystemBase",boost::python::no_init);
	boost::python::def("callScriptFunc",&Systems::callScriptFunc);
	boost::python::def("getSystem",&getSystem);
	boost::python::def("registerOperation",&registerOperation);

    boost::python::class_<Configuration>
            ("Configuration",boost::python::no_init)
                    .def("get",&Configuration::get)
                    .def("put",&Configuration::put)
                    .def("exists",&Configuration::exists);

    boost::python::class_<ConfigurationDirectory,
            boost::shared_ptr<ConfigurationDirectory>,
                boost::noncopyable>
                ("ConfigurationDirectory",boost::python::no_init)
                    .def("get",&ConfigurationDirectory::get)
                    .def("put",&ConfigurationDirectory::put)
                    .def("exists",&ConfigurationDirectory::exists);

    boost::python::def("getConfiguration",&getConfiguration , boost::python::return_value_policy<boost::python::manage_new_object>());

};
#endif /* SYSTEMSEXPORTS_H_ */
