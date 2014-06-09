/*
 * Entity.cpp
 *
 *  Created on: Oct 5, 2012
 *      Author: manicqin
 */

#include "Entity.h"

namespace EntityComponent {
	template<> EntityRegistry* Ogre::Singleton<EntityRegistry>::msSingleton = nullptr;

	EntityRegistry::data_t	EntityRegistry::getEntity(key_t const& val)
	{
		auto retVal = data_t(nullptr);
		auto iter = mEntityList.find(val);
		if (iter != std::end(mEntityList))
			retVal = iter->second;
		return retVal;
	}

	EntityRegistry::key_t	EntityRegistry::createEntity(data_t newEntity)
	{
		auto newEntityId = mCount++;
		mEntityList.insert(std::make_pair(newEntityId,newEntity));
		return newEntityId;
	}


	std::set<EntityRegistry::key_t> EntityComponent::EntityRegistry::queryEntitiesId()
	{
		std::set<EntityRegistry::key_t> retVal;
		std::for_each(std::begin(mEntityList),std::end(mEntityList),[&](registry_t::value_type const& val)
				{
					retVal.insert(val.second->getEntityType());
				}
		);
		return retVal;
	}

	std::vector<EntityRegistry::key_t> EntityComponent::EntityRegistry::getEntitiesIdsByType(
			std::size_t entityTypeId) {
		std::vector<EntityRegistry::key_t> retVal(mEntityList.size());
			std::for_each(std::begin(mEntityList),std::end(mEntityList),[&](registry_t::value_type const& val)
					{
						if(val.second->getEntityType() == entityTypeId)
						retVal.push_back(val.first);
					}
			);
			return retVal;
	}

	std::vector<EntityRegistry::data_t > EntityComponent::EntityRegistry::getEntitiesByType(
			std::size_t entityTypeId) {
		std::vector<EntityRegistry::data_t > retVal(mEntityList.size());

		std::for_each(std::begin(mEntityList),std::end(mEntityList),[&](registry_t::value_type const& val)
				{
					if(val.second->getEntityType() == entityTypeId)
					retVal.push_back(val.second);
				}
		);

		return retVal;
	}

	EntityRegistry::registry_t EntityRegistry::getRegistryView(
			std::function<bool(registry_t::value_type)> pred) {

		registry_t retVal;

		std::for_each(std::begin(mEntityList),std::end(mEntityList),[&](registry_t::value_type const& val)
				{
					if(pred(val))
					retVal.insert(val);
				}
		);

		return retVal;

	}

	void EntityRegistry::iterateRegistry(
			std::function<void(registry_t::value_type)> func) {
		std::for_each(std::begin(mEntityList),std::end(mEntityList),func);
	}
}


/* namespace EntityComponent */


