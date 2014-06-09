/*
 * PeriodicSystem.h
 *
 *  Created on: Apr 24, 2013
 *      Author: manicqin
 */

#ifndef PERIODICSYSTEM_H_
#define PERIODICSYSTEM_H_
#include "SystemBase.h"

namespace Systems {

class PeriodicSystem : public EnumedSystem <PeriodicSystem> {
public:
	PeriodicSystem();
	virtual ~PeriodicSystem();

	//virtual void operator()(std::shared_ptr<Messages>	 const& incomingMsg, ChannelHandler outChannel);
    DEFINE_CASE_PROTO(periodicalFunction);
    DEFINE_CASE_PROTO(startSystem){return MsgSwitch::caseOperationRet();}
    DEFINE_CASE_PROTO(createComponent){return MsgSwitch::caseOperationRet();}
    DEFINE_CASE_PROTO(initSystem){return MsgSwitch::caseOperationRet();	}
    DEFINE_CASE_PROTO(shutDownSystem){return MsgSwitch::caseOperationRet();	}


};

} /* namespace Systems */
#endif /* PERIODICSYSTEM_H_ */
