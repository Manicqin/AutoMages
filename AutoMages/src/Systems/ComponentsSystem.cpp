/*
 * ComponentsSystem.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: manicqin
 */
#include "ComponentsSystem.h"

#include "../EntityComponents/AllMessages.h"

#include "../EntityComponents/Entity.h"
#include "../EntityComponents/ScriptComponent.h"

#include "../Systems/ScriptManager.h"
#include "../Systems/SystemsManager.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/OgreSystem.h"
#include "../Systems/Channel.h"
#include "../Systems/LoggingSystem.h"

#include "../Utilities/rapidxml/rapidxml_print.hpp"

#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

#include <functional>
#include <iostream>
#include <memory>
using namespace EntityComponent;
namespace Systems {

ComponentsSystem::ComponentsSystem():EnumedSystem <ComponentsSystem>("ComponentsSystem"),
		mWaitOnComponentBuild(true),
		mWaitOnEntityBuild(false),
		mWaitOnSceneBuild(false),
		mWaitChannel(ChannelHandler::createChannelHandler<VoidChannel>("ComponentsSystemWaitChannel"))
	{
	// TODO Auto-generated constructor stub
	getOperations()
            .case_<CreateSceneMsg>(std::bind(&ComponentsSystem::createScene,this,std::placeholders::_1,std::placeholders::_2))
            .case_<CreateEntityMsg>(std::bind(&ComponentsSystem::createEntity,this,std::placeholders::_1,std::placeholders::_2))
			.CASE_WRAP_SCRIPT_FUNC_FOR_ENT(FocusEntity)
            .case_<ControllerMousePressedMsg>(std::bind(&ComponentsSystem::initEntity,this,std::placeholders::_1,std::placeholders::_2));
	}

ComponentsSystem::~ComponentsSystem() {
	// TODO Auto-generated destructor stub
}

DEFINE_CASE_IMP(ComponentsSystem::createComponent)
{
	auto msg = CAST_MSG_TO_TYPE(CreateComponentMsg,val);
	auto propertyTree = msg->getData();
	auto entityId = msg->getEntityId();

	if(!entityId)
		entityId = msg->getEntityId();

	LogMsg	logger(0,0,false);
	auto entItem	= EntityRegistry::getSingletonPtr()->getEntity(entityId);

	logger << boost::format("Creating components for %d") % entityId;
	EntityWorldComponent	entWrldCmp;
	PhysicsComponent		physCmp;
	LocomotionComponent		lcmtnCmp;
	RenderComponent			rndrCmp;
	//GenericComponent		gnrcCmp;
	CameraComponent			cameraCmp;
	//std::cout << *propertyTree;
	auto componentsMap = propertyTree->first_node("Components");

	auto entWrldCnfg = componentsMap->first_node(ComponentsCounter::getItemName(entWrldCmp).c_str());

	auto physCnfg 	= componentsMap->first_node(ComponentsCounter::getItemName<PhysicsComponent>().c_str());
	auto lcmtnCnfg 	= componentsMap->first_node(ComponentsCounter::getItemName<LocomotionComponent>().c_str());
	auto rndrCnfg	= componentsMap->first_node(ComponentsCounter::getItemName<RenderComponent>().c_str());

	auto scriptCnfg	= componentsMap->first_node(ComponentsCounter::getItemName<ScriptComponent>().c_str());
	auto cameraCnfg	= componentsMap->first_node(ComponentsCounter::getItemName<CameraComponent>().c_str());
	//auto gnrcCnfg 	= componentsMap->first_node(ComponentsCounter::getItemName<GenericComponent>().c_str());

	ChannelHandler channel1 = ChannelHandler::createChannelHandler<ThreadSafeChannel>("ComponentsSystem::createComponent");


	if(entWrldCnfg)
	{
		if (buildComponent(entWrldCnfg,entWrldCmp))
		{
			auto compId = ComponentRegistry<EntityWorldComponent>::getSingletonPtr()->addComponent(entWrldCmp);
			entItem->addComponent<EntityWorldComponent>(compId);
			logger << boost::format("Creating EntityWorldComponent for %d %d") % entityId % compId;
		}
	}

	auto ogreSys = SystemsManager::getSingleton().getSystemBase<OgreSystem>();
	if(rndrCnfg && ogreSys)
	{
		auto msg = boost::make_shared<CreateComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,rndrCnfg));
		msg->setEntityId(entityId);

		if(mWaitOnComponentBuild)
		{
			SystemsManager::getSingleton().getSystemBase<OgreSystem>()->getChannelIn().addMsg(ChanneledMessages::create(channel1,msg));
			waitOnChannel(channel1);
		}
		else
			SystemsManager::getSingleton().getSystemBase<OgreSystem>()->getChannelIn().addMsg(msg);
	}


	if(cameraCnfg && ogreSys)
	{
		auto msg = boost::make_shared<CreateCameraComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,cameraCnfg));

		msg->setEntityId(entityId);

		if(mWaitOnComponentBuild)
		{
			SystemsManager::getSingleton().getSystemBase<OgreSystem>()->getChannelIn().addMsg(ChanneledMessages::create(channel1,msg));
			waitOnChannel(channel1);
		}
		else
			SystemsManager::getSingleton().getSystemBase<OgreSystem>()->getChannelIn().addMsg(msg);
	}

	auto sysPhys = SystemsManager::getSingleton().getSystemBase<PhysicsSystem>();
	if(physCnfg && sysPhys)
	{
		auto msg = boost::make_shared<CreateComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,physCnfg));
		msg->setEntityId(entityId);


		if(mWaitOnComponentBuild)
		{
			SystemsManager::getSingleton().getSystemBase<PhysicsSystem>()->getChannelIn().addMsg(ChanneledMessages::create(channel1,msg));
			waitOnChannel(channel1);
		}
		else
			SystemsManager::getSingleton().getSystemBase<PhysicsSystem>()->getChannelIn().addMsg(msg);

	}

	auto sysScript = SystemsManager::getSingleton().getSystemBase<ScriptSystem>();
	if(scriptCnfg && sysScript)
	{
		auto msg = boost::make_shared<CreateComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,scriptCnfg));
		msg->setEntityId(entityId);


//		if(mWaitOnComponentBuild)
//		{
//			sysScript->getChannelIn().addMsg(ChanneledMessages::create(channel1,msg));
//			waitOnChannel(channel1);
//		}
//		else
			sysScript->getChannelIn().addMsg(msg);

	}
	/*
	 *
	 * if(initData.find("Script") != initData.not_found())
		{

			auto createCmpMsg = MAKE_SHARED_MSG_TYPE<CreateComponentMsg>(initData.get_child("Script"));
			createCmpMsg->setEntityId(getSystemEntityId());

			getSystemBase<ScriptSystem>()->getChannelIn().addMsg(createCmpMsg);

		}
	 */


	if(lcmtnCnfg)
	{
		buildComponent(lcmtnCnfg,lcmtnCmp);

	}

//	if(gnrcCnfg)
//	{
//		if (buildComponent(gnrcCnfg,gnrcCmp))
//			entItem->addComponent<GenericComponent>(ComponentRegistry<GenericComponent>::getSingletonPtr()->addComponent(gnrcCmp));
//	}

		/*
		 * Entity's components will be constructed by importance
		 * some components rely on other components and should be created first
		 * 1 : EntityWorldComponent
		 * 2 : PhysicsComponent
		 * 2 : LocomotionComponent
		 * 2 : RenderComponent
		 * 3 : GenericComponent
		 *
		 */

	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::initSystem)
{

	auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);

	//std::cout << *(msg->getData());

	auto initData = msg->getData()->first_node("ComponentsSystem");
	if(initData)
	{
		auto attr = initData->first_attribute("WaitOnComponentBuild");
		if(attr)
			mWaitOnComponentBuild = boost::lexical_cast<bool>(attr->value());

		attr = initData->first_attribute("WaitOnEntityBuild");
		if(attr)
			mWaitOnEntityBuild = boost::lexical_cast<bool>(attr->value());

		attr = initData->first_attribute("WaitOnSceneBuild");
		if(attr)
			mWaitOnSceneBuild = boost::lexical_cast<bool>(attr->value());
	}
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::shutDownSystem)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::startSystem)
{
	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::initEntity)
{
	auto msg = CAST_MSG_TO_TYPE(ControllerMousePressedMsg,val);

	//We will build a xml for a basic component
	//A basic component will have
	//entWrldCnfg
	//rndrCnfg
	//physCnfg



	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::createScene)
{
	auto msg = CAST_MSG_TO_TYPE(CreateSceneMsg,val);
		auto propertyTree = msg->getData();
		//ChannelHandler	channelScene(ChannelHandler::createChannelHandler<ThreadSafeChannel>("EntitySystemCreationChannel"));

		LogMsg() << boost::format("EntitySystem::createScene found %d Scenes") % rapidxml::count_children(propertyTree.get());
		for(auto sceneData = propertyTree->first_node("Scene"); sceneData; sceneData = sceneData->next_sibling("Scene"))
		{
			auto entMsg = boost::make_shared<CreateEntityMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,sceneData));
			createEntity(entMsg);
		}

	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::createEntity)
{

	auto msg = CAST_MSG_TO_TYPE(CreateEntityMsg,val);
		auto propertyTree = msg->getData();

		//ChannelHandler	channelScene(ChannelHandler::createChannelHandler<ThreadSafeChannel>("EntitySystemCreationChannel"));

//		for(auto sceneData = propertyTree->first_node("Scene"); sceneData; sceneData = sceneData->next_sibling())
//		{
			//LogMsg() << boost::format("EntitySystem::createComponent found %d Entities") % rapidxml::count_children(propertyTree.get(),"Entity");

			for(auto entityData = propertyTree->first_node("Entity"); entityData; entityData = entityData->next_sibling("Entity"))
			{
				//std::cout << *entityData;
				auto entReg = EntityRegistry::getSingletonPtr();
				BOOST_ASSERT_MSG(entReg,"EntityRegistry singleton was not initated");
				if(entReg)
				{

					auto entityId = msg->getEntityId();
					if(!entityId)
						entityId = entReg->createEntity();

					auto cmpMsg = boost::make_shared<CreateComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,entityData));
					cmpMsg->setEntityId(entityId);

					createComponent(cmpMsg);
//					auto sys = getSystemBase<ComponentsSystem>();
//
//					sys->getChannelIn().addMsg(ChanneledMessages::create(channelScene,cmpMsg));
				}
			}
		//}

		//waitOnChannel(channelScene);

	return	MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(ComponentsSystem::getEntityList)
{

	return	MsgSwitch::caseOperationRet();
}


}
/* namespace Systems */


