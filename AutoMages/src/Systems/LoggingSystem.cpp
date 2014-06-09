/*
 * LoggingSystem.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: manicqin
 */

#include "LoggingSystem.h"
#include "../EntityComponents/LogMessages.h"
#include "SystemsManager.h"
#include <iostream>
#include <memory>

#include <boost/format.hpp>
#include <boost/make_shared.hpp>

#include <log4cxx/logger.h>

namespace Systems {

LoggingSystem::LoggingSystem():EnumedSystem <LoggingSystem>("LoggingSystem") {
	//The only reason is to jump start the global timer

    getOperations().case_<LogMessage>([&](MsgSwitch::caseOperationArg const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
								{
									auto msg = static_cast<LogMessage *>(val.get());
									std::ostringstream 	outMsg;
                                    outMsg << msg->getTime() << "::: " << msg->getMsg();

									if(msg->isConsole())
										 LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"),outMsg.str())

									this->mOutFile << outMsg.str();

									if(msg->isNewLine())
										this->mOutFile << std::endl;

									return MsgSwitch::caseOperationRet();
								});

    getOperations()(boost::make_shared<LogMessage>("Hey boys, Hey girls, Super star DJ...... HERE WE GO!!!"),getChannelOut());
	//SystemBase::operator ()();

	mOutFile.open("Log.log");

}

LoggingSystem::~LoggingSystem() {
	mOutFile.close();
	// TODO Auto-generated destructor stub
}

//void LoggingSystem::operator ()(const std::shared_ptr<Messages>& incomingMsg,
//		ChannelHandler outChannel) {
//	MsgSwitch	msgSwitch;
//
//	msgSwitch.case_<LogMessage>([&](MsgSwitch::caseOperationArg const& val)->MsgSwitch::caseOperationRet
//					{
//						auto msg = dynamic_cast<LogMessage *>(val.get());
//						std::cout << msg->getTime() << " " << msg->getMsg() << std::endl;
//						return MsgSwitch::caseOperationRet();
//					})(incomingMsg);
//}

} /* namespace Systems */


LogMsg::LogMsg(int const& priority, int const& type, bool const& console,bool const& newLine):mPriority(priority),mType(type),mConsole(console),mNewLine(newLine)
	{

	}
	void	LogMsg::Log(std::string const && msg)
	{
		Systems::SystemsManager::getSingleton().getSystem<Systems::LoggingSystem>()->getChannelIn().addMsg(
										boost::make_shared<LogMessage>(std::move(msg),mPriority,mType,mConsole,mNewLine)
									);
	}

	void	LogMsg::operator << (boost::format const & os)
	{
		Log(std::move(os.str()));
	}

	void	LogMsg::operator << (boost::format && os)
	{
		Log(std::move(os.str()));
	}

	void	LogMsg::operator << (std::string const & msg)
	{
		Log(std::move(msg));
	}
	void	LogMsg::operator << (std::string const && msg)
		{
			Log(std::move(msg));
		}
	void LogMsg::Log_(const std::string& msg)
	{
			Log(std::move(msg));

	}

DEFINE_CASE_IMP(Systems::LoggingSystem::startSystem)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::LoggingSystem::createComponent)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::LoggingSystem::initSystem)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(Systems::LoggingSystem::shutDownSystem)
{
	return	MsgSwitch::caseOperationRet();
}
