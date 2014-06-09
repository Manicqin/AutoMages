/*
 * SystemsManager.h
 *
 *  Created on: Sep 22, 2012
 *      Author: manicqin
 */

#ifndef SYSTEMSMANAGER_H_
#define SYSTEMSMANAGER_H_
#include <OgreSingleton.h>

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <tbb/task_scheduler_init.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/task.h>

#include <map>
#include <memory>
#include <functional>
#include <array>
#include <deque>

#include "SystemBase.h"
namespace Systems {
class SystemBase;
struct sysTask
{
	sysTask(boost::shared_ptr<SystemBase>	system);

	void readQueue();

	boost::shared_ptr<SystemBase>	mSystem;
};

class SystemsManager : public Ogre::Singleton<SystemsManager>{
public:
	SystemsManager();
	virtual ~SystemsManager();

	void iterarteOver(std::function<void(std::pair<std::size_t,boost::shared_ptr<SystemBase>>)> func)
	{
		std::for_each(std::begin(mSystemVec),std::end(mSystemVec),func);
	}

	boost::shared_ptr<SystemBase> addSystem(SystemBase* newSystem);
	boost::shared_ptr<SystemBase> addSystem(boost::shared_ptr<SystemBase> newSystem);

	boost::shared_ptr<SystemBase> getSystem(std::size_t	id)
	{
		boost::shared_ptr<SystemBase> retVal = nullptr;
		auto iter = mSystemVec.find(id);
		if(iter != std::end(mSystemVec))
			retVal = iter->second;

		return retVal;
	}

	template <class T>
	boost::shared_ptr<SystemBase> getSystemBase()
	{
		boost::shared_ptr<SystemBase> retVal = nullptr;
		auto iter = mSystemVec.find(T::getItemTypeId());
		if(iter != std::end(mSystemVec))
			retVal = iter->second;

		return retVal;
	}

	template <class T>
	boost::shared_ptr<T> getSystem()
	{
		boost::shared_ptr<T> retVal = nullptr;
		auto iter = mSystemVec.find(T::getItemTypeId());
		if(iter != std::end(mSystemVec))
			retVal = boost::dynamic_pointer_cast<T>(iter->second);

		return retVal;
	}

	void wait()
	{
		mThreadGroup.join_all();
	}

	void start()
	{
		//mWorkerThread = boost::thread(&SystemsManager::iterateQueue,this);

	}
	void readQueue(boost::shared_ptr<SystemBase> const& system);
	void iterateQueue();
private:
	//boost::thread			mWorkerThread;
	boost::thread_group		mThreadGroup;
	std::list<sysTask *>	mTaskVec;
	std::map<std::size_t,boost::shared_ptr<SystemBase>> mSystemVec;
};

template <class T>
boost::shared_ptr<SystemBase> getSystemBase()
{
	return Systems::SystemsManager::getSingleton().getSystemBase<T>();
}
} /* namespace Systems */
#endif /* SYSTEMSMANAGER_H_ */
