/*
 * Channel.cpp
 *
 *  Created on: Sep 23, 2012
 *      Author: manicqin
 */

#include "Channel.h"
#include <algorithm>
#include <boost/date_time.hpp>
namespace Systems {

	Channel::~Channel() {
		//std::cout << "~Channel() " << std::endl;
	}

    void Channel::addAllMsgs(const std::vector<boost::shared_ptr<Messages> >& vecMessages) {
        std::for_each(vecMessages.begin(),vecMessages.end(),[&](boost::shared_ptr<Messages> const & msg)
				{
					this->addMsg(msg);
				});
	}

    bool Channel::getAllMsgs(std::vector<boost::shared_ptr<Messages> >& vecMessages , std::size_t msgAmount) {
        boost::shared_ptr<Messages>	tmpMsg;
		while(getMsg(tmpMsg) && msgAmount --> 0)	vecMessages.push_back(tmpMsg);
		return !vecMessages.empty();
	}

    void Channel::addMsg(const boost::shared_ptr<Messages>& newMsg) {
		addMsgImp(newMsg);
	}

    bool Channel::getMsg(boost::shared_ptr<Messages>& outMsg) {
		return getMsgImp(outMsg);
	}

}

