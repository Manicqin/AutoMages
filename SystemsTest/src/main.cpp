///*
// * main.cpp
// *
// *  Created on: Sep 22, 2012
// *      Author: manicqin
// */
//
//

#include "config.h"

#include <Systems/SystemBase.h>
#include <Systems/ScriptManager.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SystemsManager.h>
#include <Systems/ComponentsSystem.h>
#include <Systems/LoggingSystem.h>
#include <Systems/OgreSystem.h>
#include <Systems/ControllerSystem.h>
#include <Systems/PyGuiSystem.h>
#include <Systems/PeriodicSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/Channel.h>

#include <EntityComponents/Entity.h>
#include <EntityComponents/Component.h>
#include <EntityComponents/AllMessages.h>

#include <Utilities/rapidxml/rapidxml_utils.hpp>
#include <Utilities/rapidxmlUtils.h>

#include <Utilities/ConfigurationDirectory.h>

#include <boost/foreach.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>

#include <iomanip>
#include <sstream>
#include <ostream>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <boost/filesystem.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>


#include <boost/mpl/bool.hpp>
#include <boost/shared_ptr.hpp>


using namespace Systems;
using namespace std;

template <class T>
void debugComponentRegistryInfo()
{
	LogMsg() << boost::format("%x %s %d") % T::getItemTypeId() % EntityComponent::ComponentsCounter::getItemName<T>() % EntityComponent::ComponentRegistry<T>::getSingletonPtr()->size();
}

void debugPrint(std::string const& strTitle,std::vector<std::string> val)
{
	std::string retVal = "";
	LogMsg().Log(strTitle + " " + boost::lexical_cast<std::string>(val.size()) + "\n" + std::accumulate (std::begin(val),std::end(val),retVal,[](std::string const& res,std::string const& str)->std::string
						{
							return res + str + "\n";
						}));
}

void	simpleTests()
{
		debugComponentRegistryInfo<EntityComponent::PhysicsComponent>();
		debugComponentRegistryInfo<EntityComponent::EntityWorldComponent>();
		debugComponentRegistryInfo<EntityComponent::RenderComponent>();
		debugComponentRegistryInfo<EntityComponent::CameraComponent>();
		debugComponentRegistryInfo<EntityComponent::LocomotionComponent>();
	//	debugComponentRegistryInfo<EntityComponent::GenericComponent>();
		debugComponentRegistryInfo<EntityComponent::MiscComponent>();
		debugComponentRegistryInfo<EntityComponent::ScriptComponent>();

		debugPrint("Messages:",MessagesRegistry::getFamilyMembers());
		debugPrint("Components:",EntityComponent::ComponentsCounter::getFamilyMembers());
		debugPrint("Systems:",SystemsRegistry::getFamilyMembers());

		std::stringstream	msg;
		msg << std::endl;
		EntityComponent::EntityRegistry::getSingletonPtr()->iterateRegistry(
				[&](EntityComponent::EntityRegistry::registry_t::value_type const& pair)
				{

					msg << boost::format("Entity: %d  type: %#x") %  pair.first % boost::io::group(hex,showbase,pair.second->getEntityType());

					if(pair.second->begin()!=pair.second->end())
					{
						msg << boost::format("\t |_%|=25|_|_%|=10|_|_%|=10|_| \n") % "Comp Name" % "Comp Type" % "Comp Id";
					}
					else
						msg << std::endl;
					std::for_each(pair.second->begin(),pair.second->end(),[&](EntityComponent::Entity::value_type const val){
						msg << boost::format("\t\t\t |%|=27||%|=12||%|=12|| \n") % EntityComponent::ComponentsCounter::getItemName(val.first) % boost::io::group(hex,showbase,val.first) % val.second;
					});

					msg << boost::format("\t\t\t |_%|=25|_|_%|=10|_|_%|=10|_| \n") % "" % "" % "";


				}
			);
		LogMsg().Log(msg.str());
}

int main()
{

	//auto path2 = boost::filesystem::initial_path() / R"(/Configurations/ComponentsInit/OgreInit.xml)";

	log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("ViewerMain"));
	log4cxx::BasicConfigurator::configure();

	LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain"), "Start: ")
	//Declaring instances of the Singletons
	EntityComponent::ComponentRegistry<EntityComponent::PhysicsComponent> cPhysCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::LocomotionComponent> cLcmtnCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::RenderComponent> cRndrCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::EntityWorldComponent> cWrldCmpRgstr;
	//EntityComponent::ComponentRegistry<EntityComponent::GenericComponent> cGnrcCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::MiscComponent> cMiscCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::CameraComponent> cCameraCmpRgstr;
	EntityComponent::ComponentRegistry<EntityComponent::ScriptComponent> cScriptCmpRgstr;
	EntityComponent::EntityRegistry cEntRgstr;

	//Creating	Systems
	SystemsManager	mngr;
	mngr.addSystem(new LoggingSystem)		->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("LoggingSystemIn"));
	mngr.addSystem(new OgreSystem)			->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("OgreSystemIn"));
	mngr.addSystem(new ScriptSystem)		->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("ScriptSystemIn"));
	mngr.addSystem(new PhysicsSystem)		->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("PhysicsSystemIn"));
	mngr.addSystem(new ComponentsSystem)		->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("ComponentsSystemIn"));
	mngr.addSystem(new ControllerSystem)		->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("ControllerSystemIn"));
//	mngr.addSystem(new PyGuiSystem)			->setChannelIn(ChannelHandler::createChannelHandler<ThreadSafeChannel>("GuiSystemIn"));

	mngr.start();

    ConfigurationDirectory   cnfgDirectory;
    Configuration       glblCnfg;


    glblCnfg.put("ScriptsPath",Scripts_Path);
    glblCnfg.put("ConfigurationPath",Config_Path);

    cnfgDirectory.put("GlobalConfigurations",glblCnfg);

    std::string scriptsPath(Scripts_Path);
    getSystemBase<ScriptSystem>()->getChannelIn().addMsg(createFromData<InitSystemMsg>(R"(<ScriptSystem Path=")" + scriptsPath + R"(" />)").createSharedPtr());

    ChannelHandler channel2(ChannelHandler::createChannelHandler<ThreadSafeChannel>("MainChannel"));


    scriptsPath += R"(/init.py)";
    auto path = scriptsPath;
	Systems::sendMsg(ChanneledMessages::create(channel2,
            MAKE_SHARED_MSG_TYPE<ScriptExecuteFile>(path)
						));

	waitOnChannel(channel2,2000);

	simpleTests();
	boost::this_thread::sleep(boost::posix_time::seconds(600));

	LOG4CXX_INFO(log4cxx::Logger::getLogger("ViewerMain") ,"Ended");
	return 0;
}
