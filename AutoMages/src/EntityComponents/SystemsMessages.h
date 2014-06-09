/*
 * SystemsMessages.h
 *
 *  Created on: Sep 28, 2012
 *      Author: manicqin
 */

#ifndef SYSTEMSMESSAGES_H_
#define SYSTEMSMESSAGES_H_

#include "Messages.h"

#include "../Utilities/rapidxmlUtils.h"
#include <vector>
#include <memory>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>

#include <log4cxx/logger.h>

struct InitSystemMsg : public DataMsg<std::shared_ptr<rapidxml::xml_node<>>, InitSystemMsg>
{
	public:
	InitSystemMsg(std::shared_ptr<rapidxml::xml_node<>> const& data):DataMsg(data,"InitSystemMsg") {}
	virtual ~InitSystemMsg() {}
};

DataMessageDeclare2(CreateComponentMsg,std::shared_ptr<rapidxml::xml_node<>>,std::size_t,EntityId)

DataMessageDeclare(DeleteComponentMsg,std::size_t);

EmptyMessageDeclare(StartSystemMsg);
EmptyMessageDeclare(ShutDownSystemMsg);
EmptyMessageDeclare(SystemKillMsg);
EmptyMessageDeclare(SystemTouchMsg);

struct NotevMsg : public DataMsg<std::size_t, NotevMsg>
{
	public:
	NotevMsg(std::size_t const& data = getID()):DataMsg(data,"NotevMsg"){ }

	virtual ~NotevMsg(){}
	static	std::size_t getID()
	{
		static 	std::size_t retVal = 0;
		return ++retVal;
	}

	static 	boost::posix_time::ptime	mGeneralTime;

	void	printMsg(std::string const& info = ""){
	  LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"), "NotevMsg: " << getData() << " " << boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::universal_time() - mGeneralTime) << " " << info)
	}
};

#endif /* SYSTEMSMESSAGES_H_ */
