/*
 * SystemBase.h
 *
 *  Created on: Sep 22, 2012
 *      Author: manicqin
 */

#ifndef SYSTEMBASE_H_
#define SYSTEMBASE_H_

#include <boost/thread.hpp>
#include <memory>
#include "Channel.h"
#include "SystemsManager.h"
#include "../Utilities/SelfEnumClasses.h"

#include <log4cxx/logger.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/ttcclayout.h>
struct Systems_Family{};

class SystemsRegistry : public FamilyRegistry<Systems_Family>
{};

typedef MsgSwitchT<std::size_t,std::function<msgSwitch(boost::shared_ptr<Messages>,Systems::ChannelHandler)>,hashMsg> MsgSwitch;
#define DEFINE_CASE_PROTO(funcName)    MsgSwitch::caseOperationRet funcName(MsgSwitch::caseOperationArg const& val = CASE_OPERATION_ARG_DEF_VAL,Systems::ChannelHandler out = ChannelHandler())
#define DEFINE_CASE_IMP(funcName)    MsgSwitch::caseOperationRet funcName(MsgSwitch::caseOperationArg const& val,Systems::ChannelHandler out)

namespace Systems {

bool	waitOnChannel(ChannelHandler const& chann, std::size_t interval = 100,std::size_t count = 0, std::size_t connections = 1);

void	sendMsg(MsgSwitch::caseOperationArg	msg);

class SystemBase;



std::size_t getSystemId_(SystemBase const* sys);
class SystemMsgSwitch : public MsgSwitch
{
public:
//    log4cxx::LoggerPtr mLogger;
    SystemMsgSwitch(SystemBase const*	parent):mParent(parent)
    {
    }


//  //typedef MsgSwitchT<std::size_t,std::function<msgSwitch(boost::shared_ptr<Messages>,Systems::ChannelHandler)>,hashMsg> MsgSwitch;
//    msgSwitch operator()(boost::shared_ptr<Messages> inMsg,Systems::ChannelHandler channel)
//    {
//        LOG4CXX_INFO(mLogger,"Created FileAppender appender");
//        auto retVal = MsgSwitch::execute(inMsg , channel);
//        LOG4CXX_INFO(mLogger,"Created FileAppender appender");

//        return retVal;
//    }
        template <class T>
        SystemMsgSwitch& case_(caseOperation expr , T* ignore = nullptr)
		{
            auto typeId = T::getItemTypeId();
			MessagesRegistry::addSubscriber(typeId,getSystemId_(mParent));
            MsgSwitch::case_(expr,typeId);
			return *this;
        }

        SystemMsgSwitch& case_(caseOperation expr , MsgSwitch::caseOperationKey key)
        {
            auto typeId = key;
            MessagesRegistry::addSubscriber(typeId,getSystemId_(mParent));
            MsgSwitch::case_(expr,typeId);
            return *this;
        }
private:
	const SystemBase*	mParent;

};

#define CASE_FUNC(msgType , funcName)	case_<msgType>(std::bind(&funcName,this,std::placeholders::_1,std::placeholders::_2))

class SystemBase {
public:
	SystemBase();

	virtual ~SystemBase();

	ChannelHandler& getChannelIn() {
		return mChannelIn;
	}

	void setChannelIn(const ChannelHandler channelIn) {
		mChannelIn = channelIn;
	}

	ChannelHandler& getChannelOut() {
		return mChannelOut;
	}

	void setChannelOut(const ChannelHandler channelOut) {
		mChannelOut = channelOut;
	}

	void ReadQueue();

	virtual msgSwitch execute(const MsgSwitch::caseOperationArg incomingMsg) {
		auto retVal = execute(incomingMsg, getChannelOut());
		return retVal;
	}

	virtual msgSwitch execute(const MsgSwitch::caseOperationArg incomingMsg,
			ChannelHandler outChannel);

	const std::size_t getSystemId() const {
		return mSystemId;
	}

	void setSystemId(std::size_t systemId) {
		mSystemId = systemId;
	}

//	boost::thread* getWorkerThread()
//	{
//		return &mWorkerThread;
//	}

    DEFINE_CASE_PROTO(createComponentBase);
    DEFINE_CASE_PROTO(startSystemBase);
    DEFINE_CASE_PROTO(initSystemBase);
    DEFINE_CASE_PROTO(shutDownSystemBase);

    virtual	DEFINE_CASE_PROTO(periodicalFunction);
    virtual DEFINE_CASE_PROTO(createComponent) = 0;
    virtual DEFINE_CASE_PROTO(startSystem) = 0;
    virtual DEFINE_CASE_PROTO(initSystem) = 0;
    virtual DEFINE_CASE_PROTO(shutDownSystem) = 0;

	SystemMsgSwitch& getOperations(){
		return mOperations;
	}

	bool isLoopSystem() const {
		return mLoopSystem;
	}

	void setLoopSystem(bool loopSystem) {
		mLoopSystem = loopSystem;
	}

	const std::size_t getSystemEntityId() const
	{
		return mSystemEntityId;
	}

	std::size_t getReadMsgAmount() const
	{
		return mReadMsgAmount;
	}

	void setReadMsgAmount(std::size_t readMsgAmount)
	{
		mReadMsgAmount = readMsgAmount;
	}

	std::size_t getWaitTimeMilliSec() const
	{
		return mWaitTimeMilliSec;
	}

	void setWaitTimeMilliSec(std::size_t waitTimeMilliSec)
	{
		mWaitTimeMilliSec = waitTimeMilliSec;
	}

std::size_t getSystemPriority() const
{
    return mSystemPriority;
}

void setSystemPriority(std::size_t systemPriority)
{
    mSystemPriority = systemPriority;
}

private:
	ChannelHandler	mChannelIn;
	ChannelHandler	mChannelOut;
//	boost::thread	mWorkerThread;
	std::size_t		mSystemId;
	const std::size_t		mSystemEntityId;

	SystemMsgSwitch		mOperations;

	bool 			mLoopSystem;
	std::size_t 	mReadMsgAmount;
	std::size_t		mWaitTimeMilliSec;
	std::size_t		mSystemPriority;
public:
//	static boost::thread_group	mGlobalThreadGroup;
};

template <class D>
class EnumedSystem: public SystemBase , public SelfEnumedClass<Systems_Family,D>
{
public:
	EnumedSystem(std::string const& systemName):SelfEnumedClass<Systems_Family,D>(systemName)
	{
		setSystemId(SelfEnumedClass<Systems_Family,D>::getItemTypeId());
	}
};


} /* namespace Systems */
#endif /* SYSTEMBASE_H_ */
