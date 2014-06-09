/*
 * ComponentUtils.h
 *
 *  Created on: Oct 12, 2012
 *      Author: manicqin
 */

#ifndef COMPONENTUTILS_H_
#define COMPONENTUTILS_H_
#include <map>
#include <string>
#include <iostream>

#include <boost/property_tree/ptree_fwd.hpp>

#include <boost/preprocessor.hpp>
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>

#include "../Utilities/SelfEnumClasses.h"
#include "../Utilities/MacroUtils.h"
#include "../Utilities/rapidxml/rapidxml.hpp"
#define GET_COMP_REG(T)	EntityComponent::ComponentRegistry<T>::getSingleton()

#define EmptyComponent(compName) struct compName : public EntityComponent::EnumComponents<compName> \
{ \
	public:	\
	compName():EnumComponents(#compName) { }\
	virtual ~compName(){}\
};

#define ComponentDeclare1(compName , MT1 , Mx1)	struct compName : public EntityComponent::EnumComponents<compName> \
{ \
	public:	\
	compName():EnumComponents(#compName) { nullifier(NameProperty(Mx1)); }\
	virtual ~compName(){}	\
	DeclareProperty(MT1,Mx1)	\
};

#define ComponentDeclare2(compName , MT1 , Mx1 , MT2 , Mx2)	struct compName : public EntityComponent::EnumComponents<compName> \
{ \
	public:	\
	compName():EnumComponents(#compName) { nullifier(NameProperty(Mx1)); nullifier(NameProperty(Mx2));}\
	virtual ~compName(){}	\
	DeclareProperty(MT1,Mx1)	\
	DeclareProperty(MT2,Mx2)	\
};

#define ComponentDeclare3(compName , MT1 , Mx1 , MT2 , Mx2 , MT3 , Mx3)	struct compName : public EntityComponent::EnumComponents<compName> \
{ \
	public:	\
	compName():EnumComponents(#compName) { nullifier(NameProperty(Mx1)); nullifier(NameProperty(Mx2)); nullifier(NameProperty(Mx3));}\
	virtual ~compName(){}	\
	DeclareProperty(MT1,Mx1)	\
	DeclareProperty(MT2,Mx2)	\
	DeclareProperty(MT3,Mx3)	\
};

#define ComponentDeclare4(compName , MT1 , Mx1 , MT2 , Mx2 , MT3 , Mx3 , MT4 , Mx4)	struct compName : public EntityComponent::EnumComponents<compName> \
{ \
	public:	\
	compName():EnumComponents(#compName) { nullifier(NameProperty(Mx1)); nullifier(NameProperty(Mx2)); nullifier(NameProperty(Mx3)); nullifier(NameProperty(Mx4)); }\
	virtual ~compName(){}	\
	DeclareProperty(MT1,Mx1)	\
	DeclareProperty(MT2,Mx2)	\
	DeclareProperty(MT3,Mx3)	\
	DeclareProperty(MT4,Mx4)	\
};

template <class V>
V readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const &  nodeName);


template<typename T>
std::map<std::string,T>	convertXmlAttributeToMap(boost::property_tree::ptree const & propertyTree, std::string const & nodeName);

template <class V>
V readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName);

template <class V>
V readVectorXml(rapidxml::xml_node<> const * node);
struct	Components_Family{};

namespace EntityComponent {


struct ComponentsCounter : public FamilyRegistry<Components_Family>
{
};

template<class D>
struct EnumComponents : public SelfEnumedClass<Components_Family,D>
{
	EnumComponents(){}
	EnumComponents(std::string const& compName):SelfEnumedClass<Components_Family,D>(compName){}

};

}


#endif /* COMPONENTUTILS_H_ */
