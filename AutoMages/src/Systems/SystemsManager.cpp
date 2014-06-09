/*
 * SystemsManager.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: manicqin
 */

#include "SystemsManager.h"
#include "SystemBase.h"

#include <tbb/parallel_for_each.h>

#include <algorithm>
#include <log4cxx/logger.h>
namespace Systems {

sysTask::sysTask(boost::shared_ptr<SystemBase>	system):mSystem(system)
{

}

void sysTask::readQueue()
{
	while(true){
	//	LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"), "sysTask::readQueue: " << SystemsRegistry::getItemName(mSystem->getSystemId()))

		if(mSystem->getChannelIn().isChannelOpen())
		{
			if(mSystem->getReadMsgAmount() > 1 )
			{
				std::vector<MsgSwitch::caseOperationArg >	tmp;
				tmp.reserve(mSystem->getReadMsgAmount());
				if(mSystem->getChannelIn().getAllMsgs(tmp))
				{
					std::for_each(std::begin(tmp),std::end(tmp),[&](MsgSwitch::caseOperationArg const& val){
						mSystem->execute(val,mSystem->getChannelOut());
					});
				}
			}
			else
			{
				MsgSwitch::caseOperationArg	tmp = CASE_OPERATION_ARG_DEF_VAL;
				if(mSystem->getChannelIn().getMsg(tmp))
				{
					mSystem->execute(tmp,mSystem->getChannelOut());
				}
			}
		}

		if(!mSystem->getWaitTimeMilliSec())
			boost::this_thread::yield();
		else
			boost::this_thread::sleep(boost::posix_time::milliseconds(mSystem->getWaitTimeMilliSec()));
	}
}


template<>
SystemsManager*
	Ogre::Singleton<SystemsManager>::msSingleton = nullptr;

	SystemsManager::SystemsManager() {

	}

	SystemsManager::~SystemsManager() {

	}
	boost::shared_ptr<SystemBase> SystemsManager::addSystem(SystemBase* newSystem)
	{
		auto sysTmp = boost::shared_ptr<SystemBase>(newSystem);
		addSystem(sysTmp);
		return sysTmp;
	}
	boost::shared_ptr<SystemBase> SystemsManager::addSystem(boost::shared_ptr<SystemBase> newSystem)
	{
		mSystemVec.insert(std::make_pair(newSystem->getSystemId(),newSystem));
		auto tmp = new sysTask(newSystem);
		mTaskVec.push_back(tmp);
		mThreadGroup.add_thread(new boost::thread(&sysTask::readQueue,tmp));

		return newSystem;
	}
	void SystemsManager::readQueue(boost::shared_ptr<SystemBase> const& system) {
		try{
			//while(system->isLoopSystem())
			{

				if(system->getChannelIn().isChannelOpen())
				{
					if(system->getReadMsgAmount() > 1 )
					{
						std::vector<MsgSwitch::caseOperationArg >	tmp;
						tmp.reserve(system->getReadMsgAmount());
						if(system->getChannelIn().getAllMsgs(tmp,system->getReadMsgAmount()))
						{
							//std::cout << "SystemsManager::readQueue " << SystemsRegistry::getItemName(system->getSystemId()) << " " << tmp.size() << std::endl;
							std::for_each(std::begin(tmp),std::end(tmp),[&](MsgSwitch::caseOperationArg const& val){
								system->execute(val,system->getChannelOut());
							});
						}
					}
					else
					{
						MsgSwitch::caseOperationArg	tmp = CASE_OPERATION_ARG_DEF_VAL;
						if(system->getChannelIn().getMsg(tmp))
						{
							system->execute(tmp,system->getChannelOut());
						}
					}
				}

//				if(!system->getWaitTimeMilliSec())
//					boost::this_thread::yield();
//				else
//					boost::this_thread::sleep(boost::posix_time::milliseconds(system->getWaitTimeMilliSec()));
			}
		}
		catch(...)
		{

			std::cout << "System " << SystemsRegistry::getItemName(system->getSystemId()) << " has crushed, fuck you"<<std::endl;
		}
	}

//	void SystemsManager::iterateQueue()
//	{
//
//
//
////		try{
//
//		tbb::task_scheduler_init	init(mSystemVec.size());
//		std::map<std::size_t,tbb::task*>	tasks;
//			while(true){
////
////				tbb::parallel_for_each(std::begin(mSystemVec),std::end(mSystemVec),
////						[&](std::pair<std::size_t,boost::shared_ptr<SystemBase>> const& val)
////								{
////									readQueue(val.second);
////								});
//
//
////				std::for_each(std::begin(mSystemVec),std::end(mSystemVec),
////						[&](std::pair<std::size_t,boost::shared_ptr<SystemBase>> const& val)
////								{
////									readQueue(val.second);
////								});
//
//			}
////		}
////		catch(...)
////		{
////
////			std::cout << "so yhe we crushed, fuck you"<<std::endl;
////		}
//
//
//	}
					} /* namespace Systems */


