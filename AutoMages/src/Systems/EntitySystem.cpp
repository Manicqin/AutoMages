/*
 * EntitySystem.cpp
 *
 *  Created on: Jul 20, 2013
 *      Author: manicqin
 */

#include "EntitySystem.h"
#include "../EntityComponents/AllMessages.h"

#include "../EntityComponents/Entity.h"

#include "../Systems/ComponentsSystem.h"
#include "../Systems/LoggingSystem.h"
#include "../Systems/ScriptManager.h"

#include "../Utilities/rapidxml/rapidxml_utils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

using namespace EntityComponent;

namespace Systems {

EntitySystem::EntitySystem():EnumedSystem <EntitySystem>("EntitySystem"),
		mWaitOnEntityBuild(true){
	// TODO Auto-generated constructor stub


}

EntitySystem::~EntitySystem() {
	// TODO Auto-generated destructor stub
}

DEFINE_CASE_IMP(EntitySystem::startSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(EntitySystem::initSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(EntitySystem::shutDownSystem)
{
	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(EntitySystem::createComponent)
{
	auto msg = CAST_MSG_TO_TYPE(CreateComponentMsg,val);
	auto propertyTree = msg->getData();
	ChannelHandler	channelScene(ChannelHandler::createChannelHandler<ThreadSafeChannel>("EntitySystemCreationChannel"));

	for(auto sceneData = propertyTree->first_node("Scene"); sceneData; sceneData = sceneData->next_sibling())
	{
		LogMsg() << boost::format("EntitySystem::createComponent found %d Entities") % rapidxml::count_children(sceneData);

		for(auto entityData = sceneData->first_node("Entity"); entityData; entityData = entityData->next_sibling())
		{
			auto entReg = EntityRegistry::getSingletonPtr();
			BOOST_ASSERT_MSG(entReg,"EntityRegistry singleton was not initated");
			if(entReg)
			{
				auto entityId = entReg->createEntity();

				auto cmpMsg = boost::make_shared<CreateComponentMsg>(std::shared_ptr<rapidxml::xml_node<>>(propertyTree,entityData));
				cmpMsg->setEntityId(entityId);

				auto sys = getSystemBase<ComponentsSystem>();

				sys->getChannelIn().addMsg(ChanneledMessages::create(channelScene,cmpMsg));
			}
		}
	}

	waitOnChannel(channelScene);
	return MsgSwitch::caseOperationRet();
}

} /* namespace Systems */
