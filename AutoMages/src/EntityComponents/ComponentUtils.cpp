/*
 * ComponentUtils.cpp
 *
 *  Created on: Oct 12, 2012
 *      Author: manicqin
 */
#include "ComponentUtils.h"
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreVector4.h>
#include <bullet/LinearMath/btVector3.h>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include "../Utilities/rapidxml/rapidxml.hpp"

template <>
Ogre::Vector3 readVectorXml(rapidxml::xml_node<> const * node)
{
	auto	x = boost::lexical_cast<Ogre::Real>(node->first_attribute("x")->value());
	auto	y = boost::lexical_cast<Ogre::Real>(node->first_attribute("y")->value());
	auto	z = boost::lexical_cast<Ogre::Real>(node->first_attribute("z")->value());

	return Ogre::Vector3(x,y,z);
}


template <>
Ogre::Vector4 readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	Ogre::Vector4 retVal;
	boost::property_tree::ptree const& dom = propertyTree.get_child(nodeName);
	retVal.x = dom.get<Ogre::Real>("x");
	retVal.y = dom.get<Ogre::Real>("y");
	retVal.z = dom.get<Ogre::Real>("z");
	retVal.w = dom.get<Ogre::Real>("w");

	return retVal;
}

template <>
Ogre::Vector3 readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	Ogre::Vector3 retVal;
	boost::property_tree::ptree const& dom = propertyTree.get_child(nodeName);
	retVal.x = dom.get<Ogre::Real>("x");
	retVal.y = dom.get<Ogre::Real>("y");
	retVal.z = dom.get<Ogre::Real>("z");

	return retVal;
}

template<typename T>
std::map<std::string,T>	convertXmlAttributeToMap(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	std::map<std::string,T> attributes;
	BOOST_FOREACH(boost::property_tree::ptree::value_type const & v, propertyTree.get_child(nodeName + ".<xmlattr>"))
	{
		attributes.insert(std::make_pair(v.first.data(), v.second.get_value<T>()));
	}
	return attributes;
}

template <>
Ogre::Vector4 readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	auto attributes = convertXmlAttributeToMap<Ogre::Real>(propertyTree,nodeName);
	return Ogre::Vector4(attributes["x"],attributes["y"],attributes["z"],attributes["w"]);
}

template <>
Ogre::Vector3 readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	auto attributes = convertXmlAttributeToMap<Ogre::Real>(propertyTree,nodeName);
	return Ogre::Vector3(attributes["x"],attributes["y"],attributes["z"]);
}

template <>
Ogre::Vector2 readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	auto attributes = convertXmlAttributeToMap<Ogre::Real>(propertyTree,nodeName);
	return Ogre::Vector2(attributes["x"],attributes["y"]);
}

template <>
Ogre::Vector2 readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	Ogre::Vector2 retVal;
	boost::property_tree::ptree const & dom = propertyTree.get_child(nodeName);
	retVal.x = dom.get<Ogre::Real>("x");
	retVal.y = dom.get<Ogre::Real>("y");

	return retVal;
}

template<>
btVector3 readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	boost::property_tree::ptree const & dom = propertyTree.get_child(nodeName);

	return	btVector3(dom.get<btScalar>("x"),dom.get<btScalar>("y"),dom.get<btScalar>("z"));
}
template<>
btVector4 readVectorGen3(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	boost::property_tree::ptree const & dom = propertyTree.get_child(nodeName);

	return	btVector4(dom.get<btScalar>("x"),dom.get<btScalar>("y"),dom.get<btScalar>("z"),dom.get<btScalar>("w"));
}

template <>
btVector4 readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	auto attributes = convertXmlAttributeToMap<btScalar>(propertyTree,nodeName);
	return btVector4(attributes["x"],attributes["y"],attributes["z"],attributes["w"]);
}

template <>
btVector3 readVectorXml(boost::property_tree::ptree const & propertyTree, std::string const & nodeName)
{
	auto attributes = convertXmlAttributeToMap<btScalar>(propertyTree,nodeName);
	return btVector3(attributes["x"],attributes["y"],attributes["z"]);
}

template <>
btVector3 readVectorXml(rapidxml::xml_node<> const * node)
{
	auto	x = boost::lexical_cast<btScalar>(node->first_attribute("x")->value());
	auto	y = boost::lexical_cast<btScalar>(node->first_attribute("y")->value());
	auto	z = boost::lexical_cast<btScalar>(node->first_attribute("z")->value());

	return btVector3(x,y,z);
}

//For components we need a different way to ID them
//And if we start from 0 we cannot << so if mItemCounter == 0 we increment and return
//I tried to specialize the template with 1 but failed
template<>
std::size_t FamilyRegistry<Components_Family>::getNewId(int modifier)
{
	std::size_t	retVal = (mItemsCounter==0)?++mItemsCounter:mItemsCounter;
	mItemsCounter <<= 1;
	return retVal;
}

//std::size_t							EntityComponent::ComponentsCounter::mCompCounter = 1;
//std::map<std::size_t,std::string>	EntityComponent::ComponentsCounter::mRegistry;


