/*
 * EntityRegister.h
 *
 *  Created on: Jun 10, 2012
 *      Author: manicqin
 */

#ifndef ENTITYREGISTER_H_
#define ENTITYREGISTER_H_

#include <OgreSingleton.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/optional.hpp>
#include <limits>

using namespace boost::multi_index;

typedef		std::string 	entityName_t;
typedef		unsigned int	entityId_t;

struct entityNameTag{};
struct entityIdTag{};

struct	entityName2Id
{
	entityName2Id(entityName_t const&	entityNameVal,	entityId_t const& entityIdVal):
		mEntityName(entityNameVal),
		mEntityId(entityIdVal)
	{}
	entityName_t	mEntityName;
	entityId_t 		mEntityId;
};


typedef	boost::multi_index_container<entityName2Id,
		indexed_by<
			ordered_unique<tag<entityNameTag>, BOOST_MULTI_INDEX_MEMBER(entityName2Id,entityName_t,mEntityName)>,
			ordered_unique<tag<entityIdTag>, BOOST_MULTI_INDEX_MEMBER(entityName2Id,entityId_t,mEntityId)>
		>
	> EntityName2IdMap_t;


class EntityRegister : public Ogre::Singleton<EntityRegister>{
public:
	EntityRegister();
	virtual ~EntityRegister();

	entityId_t registerNewEntity(entityName_t newEntityName)
	{
		if (mFreeId < std::numeric_limits<decltype(mFreeId)>::max())
		{
			if(getIdByNameOpt(newEntityName))
			{
				throw "registerNewEntity Entity name is taken";
			}
			else
			{
				mCont.insert(entityName2Id(newEntityName,mFreeId));
				++mFreeId;
			}
		}
		else
		{
			throw	"registerNewEntity got to the maximum capacity";
		}

		return 0;
	}

	entityId_t getIdByName(entityName_t strName) const
	{
		auto &nameView = get<entityNameTag>(mCont);
		auto iter = nameView.find(strName);

		if (iter == nameView.end())
		{
			throw "couldn't find entity in the list";
		}

		return iter->mEntityId;
	}

	entityName_t getNameById(entityId_t iId) const
	{
		auto &idView = get<entityIdTag>(mCont);
		auto iter = idView.find(iId);

		if (iter == idView.end())
		{
			throw "couldn't find entity in the list";
		}

		return iter->mEntityName;
	}

	boost::optional<entityId_t>	getIdByNameOpt(entityName_t strName) const
	{
		auto &nameView = get<entityNameTag>(mCont);
		auto iter = nameView.find(strName);
		boost::optional<entityId_t>	retVal;

		if (iter != nameView.end())
		{
			retVal.reset(iter->mEntityId);
		}

		return retVal;
	}

	boost::optional<entityName_t> getNameByIdOpt(entityId_t iId) const
	{
		auto &idView = get<entityIdTag>(mCont);
		auto iter = idView.find(iId);
		boost::optional<entityName_t> retVal;

		if (iter != idView.end())
		{
			retVal.reset(iter->mEntityName);
		}

		return retVal;
	}
private:
	EntityName2IdMap_t	mCont;
	entityId_t	mFreeId;
};

#endif /* ENTITYREGISTER_H_ */
