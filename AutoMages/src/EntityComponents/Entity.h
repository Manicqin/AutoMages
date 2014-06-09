/*
 * Entity.h
 *
 *  Created on: Oct 5, 2012
 *      Author: manicqin
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <OgreSingleton.h>

#include <map>
#include <set>
#include <memory>

namespace EntityComponent {

#define	GET_ENTITY(x)	EntityComponent::EntityRegistry::getSingleton().getEntity(x)

class Entity
{
public:

    typedef std::size_t                                   		key_type;
    typedef std::size_t                                   		mapped_type;
    typedef std::pair<const std::size_t, std::size_t>     		value_type;
    typedef std::map<std::size_t,std::size_t>::iterator    		iterator;
    typedef std::map<std::size_t,std::size_t>::const_iterator	const_iterator;
	Entity()
	{

	}

	template <class Comp>
	void addComponent(std::size_t const& cmp,std::size_t const& id = Comp::getItemTypeId())
	{
		mCmpList.insert(std::make_pair(id,cmp));
	}

	void removeComponent(std::size_t const& id);
	std::size_t	getComponent(std::size_t const& id)
	{
		auto cmp = mCmpList.find(id);
		return cmp != mCmpList.end() ? cmp->second : 0;
	}

	template <class Comp>
	std::size_t	getComponent()
	{
		auto cmp = mCmpList.find(Comp::getItemTypeId());
		return cmp != mCmpList.end() ? cmp->second : 0;
	}

	std::size_t	getEntityType()
	{
		std::size_t	retVal = 0;
		retVal = std::accumulate(std::begin(mCmpList),std::end(mCmpList),retVal,
				[](std::size_t & acc , std::pair<std::size_t,std::size_t> const& val)
				{
					acc += val.first;
					return acc;
				});
		return retVal;
	}

	iterator	begin() { return mCmpList.begin(); }
	iterator	end() { return mCmpList.end(); }
	const_iterator	cbegin() const { return mCmpList.cbegin(); }
	const_iterator  cend() const { return mCmpList.cend(); }
private:
	std::map<std::size_t,std::size_t>	mCmpList;
};

class EntityRegistry : public Ogre::Singleton<EntityRegistry>
{
public:
	typedef	std::shared_ptr<Entity>	data_t;
	typedef	std::size_t				key_t;

	typedef std::map<key_t,data_t>	registry_t;

	EntityRegistry(){}
	data_t					getEntity(std::size_t const& val);
	key_t					createEntity(data_t newEntity = std::make_shared<Entity>());
	std::vector<key_t>		getEntitiesIdsByType(std::size_t entityTypeId);
	std::vector<data_t >	getEntitiesByType(key_t entityTypeId);
	std::set<std::size_t>	queryEntitiesId();
	std::size_t				getEntitiesCount() { return mCount; }

	void					iterateRegistry(std::function<void(registry_t::value_type)> func);
	registry_t	getRegistryView(std::function<bool(registry_t::value_type)> pred = [](registry_t::value_type){return true;});
private:
	registry_t	mEntityList;
	std::size_t	mCount = 1;
};



} /* namespace EntityComponent */
#endif /* ENTITY_H_ */
