/*
 * Component.h
 *
 *  Created on: Oct 5, 2012
 *      Author: manicqin
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_
#include <btBulletDynamicsCommon.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSingleton.h>
//#include <OIS/OIS.h>
//#include <SdkCameraMan.h>
#include <map>
#include "../Utilities/rapidxml/rapidxml.hpp"

#include "ComponentUtils.h"

namespace EntityComponent {


EmptyComponent(MiscComponent);

ComponentDeclare4(PhysicsComponent,btScalar,Mass,
									std::shared_ptr<btRigidBody>,RigidBody,
									int,ObjectHash,
									btVector3,Envelope);
ComponentDeclare3(EntityWorldComponent,Ogre::Vector3,Orientation,
										Ogre::Vector3,Scale,
										Ogre::Vector3,Position);
ComponentDeclare2(LocomotionComponent,Ogre::Vector3,Transition,
										Ogre::Quaternion,Spin);
ComponentDeclare1(RenderComponent,std::shared_ptr<Ogre::SceneNode>,SceneNode);
//ComponentDeclare1(GenericComponent,rapidxml::xml_node<>*,Data);
ComponentDeclare1(CameraComponent,Ogre::Camera*,Camera);


template<class T>
class ComponentRegistry : public Ogre::Singleton<ComponentRegistry<T> >
{
public:
	ComponentRegistry():mCount(1) {}
	~ComponentRegistry() {}
	std::size_t addComponent(boost::shared_ptr<T> cmp)
	{
		std::size_t retVal = mCount++;
		mCmpMap.insert(std::make_pair(retVal,cmp));
		return retVal;
	}
	std::size_t addComponent(T const& cmp)
	{
		std::size_t retVal = mCount++;
		mCmpMap.insert(std::make_pair(retVal,boost::shared_ptr<T>(new T(cmp))));
		return retVal;
	}
	void removeComponent(std::size_t id);
	boost::shared_ptr<T>	getComponent(std::size_t id)
	{
		auto cmp = mCmpMap.find(id);
		return cmp != mCmpMap.end() ? cmp->second : boost::shared_ptr<T>(nullptr);
	}

	boost::shared_ptr<const T>	getComponentView(std::size_t id)
	{
		auto cmp = mCmpMap.find(id);
		return cmp != mCmpMap.end() ? cmp->second : boost::shared_ptr<const T>(nullptr);
	}
	std::size_t	size()
	{
		return mCmpMap.size();
	}

	void iterarteOver(std::function<void(std::pair<std::size_t,boost::shared_ptr<T>>)> func)
	{
		std::for_each(std::begin(mCmpMap),std::end(mCmpMap),func);
	}
private:
	std::map<std::size_t,boost::shared_ptr<T>>	mCmpMap;
	std::size_t	mCount;
};

template <class T>
bool	buildComponent(rapidxml::xml_node<>* const config,T & cmp);

} /* namespace EntityComponent */
#endif /* COMPONENT_H_ */
