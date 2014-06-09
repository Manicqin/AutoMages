#ifndef SELFENUM_H_
#define SELFENUM_H_
#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

std::vector<std::string> getFamilyMembers(std::map<std::size_t,std::string>	family);
template <class Family>
class FamilyRegistry
{
public:
	static std::size_t getNewId(int modifier = 0)
	{
		std::size_t	retVal = mItemsCounter + modifier;
		mItemsCounter += 10;
		return retVal;
	}
	static	void	addItem(std::size_t key,std::string const & itemName)
	{
		mRegistry.insert(std::make_pair(key,itemName));
	}

	template <class T>
	static 	std::string	getItemName(T = T())
	{
		return getItemName(T::getItemTypeId());
	}

	static 	std::string	getItemName(std::size_t typeId)
	{
		std::string retVal = "";
		auto iter = mRegistry.find(typeId);
		if (iter != std::end(mRegistry))
			retVal = iter->second;
		return retVal;
	}

	static std::vector<std::string> getFamilyMembers()
	{
		std::vector<std::string>	retVal;
		for(auto iter = std::begin(mRegistry); iter != std::end(mRegistry);++iter)
			retVal.push_back(boost::lexical_cast<std::string>(iter->first)+":"+iter->second);
		return retVal;
	}

	static std::size_t	getItemIdByName(std::string const& sysName)
	{
		auto retVal = std::find_if(std::begin(mRegistry),std::end(mRegistry),[=](std::pair<std::size_t,std::string> const& val)
				{
					return val.second == sysName;
				});

		if (retVal != std::end(mRegistry))
			return retVal->first;
		else
			return 0;
	}
	private:
		static std::map<std::size_t,std::string>	mRegistry;
		static std::size_t	mItemsCounter;
};
template <class Family>
std::map<std::size_t,std::string> FamilyRegistry<Family>::mRegistry;

template <class Family>
std::size_t FamilyRegistry<Family>::mItemsCounter = 1;

template<class Family,class Derived,int Version = 0>
class SelfEnumedClass
{
public:
	typedef	Family	ClassFamily;
	typedef	Derived	ClassDeriver;

//	SelfEnumedClass(){
//		if(SelfEnumedClass::mItemId == 0)
//		{
//			SelfEnumedClass::mItemId = FamilyRegistry<Family>::getNewId(Version);
//		}
//	}

	SelfEnumedClass(std::string const& itemName = "")
	{
		if(SelfEnumedClass::mItemId == 0)
		{
			SelfEnumedClass::mItemId = FamilyRegistry<Family>::getNewId(Version);
		}
		if(!itemName.empty())
			FamilyRegistry<Family>::addItem(SelfEnumedClass::mItemId,itemName);

	}

	virtual ~SelfEnumedClass(){}

	static std::size_t	getItemTypeId()
	{
		if(SelfEnumedClass::mItemId == 0)
		{
			SelfEnumedClass::mItemId = FamilyRegistry<Family>::getNewId(Version);
		}
		return mItemId;
	}

private:
	static std::size_t	mItemId;

};
template<class Family,class Derived,int Version>
std::size_t	SelfEnumedClass<Family,Derived,Version>::mItemId = 0;

#endif
