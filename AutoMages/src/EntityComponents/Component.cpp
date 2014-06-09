/*
 * Component.cpp
 *
 *  Created on: Oct 5, 2012
 *      Author: manicqin
 */

#include "Component.h"
#include <boost/foreach.hpp>
namespace EntityComponent {

	template<>
		ComponentRegistry<PhysicsComponent>*
		Ogre::Singleton<ComponentRegistry<PhysicsComponent>>::msSingleton = nullptr;
	template<>
		ComponentRegistry<EntityWorldComponent>*
		Ogre::Singleton<ComponentRegistry<EntityWorldComponent>>::msSingleton = nullptr;
	template<>
		ComponentRegistry<LocomotionComponent>*
		Ogre::Singleton<ComponentRegistry<LocomotionComponent>>::msSingleton = nullptr;
	template<>
		ComponentRegistry<RenderComponent>*
		Ogre::Singleton<ComponentRegistry<RenderComponent>>::msSingleton = nullptr;

//	template<>
//		ComponentRegistry<GenericComponent>*
//		Ogre::Singleton<ComponentRegistry<GenericComponent>>::msSingleton = nullptr;

	template<>
	ComponentRegistry<MiscComponent>*
			Ogre::Singleton<ComponentRegistry<MiscComponent>>::msSingleton = nullptr;

	template<>
	ComponentRegistry<CameraComponent>*
			Ogre::Singleton<ComponentRegistry<CameraComponent>>::msSingleton = nullptr;

	template <> bool	buildComponent(rapidxml::xml_node<>* const config,EntityWorldComponent & cmp)
	{
		if(config->first_node("Orientation"))
			cmp.setOrientation(readVectorXml<Ogre::Vector3>(config->first_node("Orientation")));

		if(config->first_node("Scale"))
					cmp.setScale(readVectorXml<Ogre::Vector3>(config->first_node("Scale")));

		if(config->first_node("Position"))
					cmp.setPosition(readVectorXml<Ogre::Vector3>(config->first_node("Position")));

		return true;
	}

	template <> bool	buildComponent(rapidxml::xml_node<>* const config,LocomotionComponent & cmp)
	{
		return false;
	}

	template <> bool	buildComponent(rapidxml::xml_node<>* const config,PhysicsComponent & cmp)
	{
		return false;
	}

	template <> bool	buildComponent(rapidxml::xml_node<>* const config,RenderComponent & cmp)
	{
		return false;
	}

//	template <> bool	buildComponent(rapidxml::xml_node<>* const config,GenericComponent & cmp)
//	{
//		cmp.setData(config);
//		return true;
//	}
} /* namespace EntityComponent */
