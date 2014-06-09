/*
 * OgreSystem.cpp
 *
 *  Created on: Oct 27, 2012
 *      Author: manicqin
 */


#include "OgreSystem.h"

#include "LoggingSystem.h"
#include "GuiSystem.h"

#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/AllMessages.h"
#include "../EntityComponents/SystemsMessages.h"
#include "../EntityComponents/GraphicsMessages.h"
#include "../EntityComponents/OgreMiscComponent.h"
#include "../EntityComponents/ComponentUtils.h"
#include "../EntityComponents/GuiComponent.h"

#include "../Utilities/Physics/Euler.h"
#include "../Utilities/OgreUtils.h"
#include "../Utilities/ConfigurationDirectory.h"

#include "../Utilities/rapidxml/rapidxml.hpp"
#include "../Utilities/rapidxml/rapidxml_iterators.hpp"
#include "../Utilities/Vector3.h"

#include <Ogre.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/barrier.hpp>

#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <CEGUI.h>
namespace Systems {

class OgreLogConverter : public Ogre::LogListener
{
	void messageLogged( const Ogre::String& message,
			Ogre::LogMessageLevel lml,
			bool maskDebug,
			const Ogre::String &logName,
			bool& skipThisMessage )
	{
		if(!skipThisMessage)
			LogMsg(lml,0,false).Log(logName + " " + message);
	}
};

OgreSystem::OgreSystem():EnumedSystem <OgreSystem>("OgreSystem"),
		mResourcesCfg(Ogre::StringUtil::BLANK),
		mPluginsCfg(Ogre::StringUtil::BLANK),mRoot(0),
		mCamera(0),
		mSceneMgr(0),
		mRenderOne(false),
		mRenderInSameThread(true),
		mShowConfiguration(false),
		mFrameIdx(0)

{

	mOgreComponent = boost::shared_ptr<EntityComponent::OgreMiscComponent>(new EntityComponent::OgreMiscComponent);
	mOgreComponentId = GET_COMP_REG(EntityComponent::MiscComponent).addComponent(mOgreComponent);

//	mScriptComponent = std::make_shared<EntityComponent::ScriptComponent>();

	auto log = new Ogre::Log("OgreLog",false,true);
	log->addListener(new OgreLogConverter);
	mLogManager.setDefaultLog(log);
	mWaitForInit = new boost::barrier(2);


	getOperations().CASE_FUNC(PositionChangedMsg,OgreSystem::PositionChanged)
	.CASE_FUNC(CreateCameraComponentMsg,OgreSystem::createCameraComponent)
	.CASE_FUNC(CastRayMsg,OgreSystem::castRay)
	.CASE_FUNC(StartAnimationMsg,OgreSystem::startAnimation)
	.CASE_FUNC(MoveCameraMsg,OgreSystem::moveCamera);
	//.CASE_WRAP_SCRIPT_FUNC(ControllerBufferedMousePressedMsg)
	//.CASE_WRAP_SCRIPT_FUNC(ControllerSubscribedKeyMsg);
	//.case_<ControllerMousePressedMsg>(std::bind(wrapFunc<ControllerMousePressedMsg>, "onControllerMousePressedMsg" , getSystemEntityId() , std::placeholders::_1));
	//.case_<ControllerKeyMsg>(std::bind(wrapFunc<ControllerKeyMsg>, "test2" , getSystemEntityId() , std::placeholders::_1))
	//.case_<ControllerMousePressedMsg>(std::bind(wrapFunc<ControllerMousePressedMsg>, "test2" , getSystemEntityId() , std::placeholders::_1));
}

OgreSystem::~OgreSystem() {
	//	auto miscStorage = GET_COMP_REG(EntityComponent::MiscComponent).getComponent(mOgreComponentId);
	//	auto storage = std::dynamic_pointer_cast<EntityComponent::OgreMiscComponent>(miscStorage);



}

bool OgreSystem::configure(void)
{
	// Show the configuration dialog and initialize the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if((mShowConfiguration && mRoot->showConfigDialog()) ||
			(mRoot->restoreConfig() || mRoot->showConfigDialog()))
	{
		// If returned true, user clicked OK so initialize
		// Here we choose to let the system create a default rendering window by passing 'true'

		mOgreComponent->mWindow = mRoot->initialise(true, "AutoMages");

		return true;
	}

	return false;

}

void OgreSystem::chooseSceneManager(void)
{
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE,"Default");
}

DEFINE_CASE_IMP(OgreSystem::createCameraComponent)
{
	auto msg = CAST_MSG_TO_TYPE(CreateCameraComponentMsg,val);
	auto ent = GET_ENTITY(msg->getEntityId());

	createCamera(msg->getData());
	auto cam = mSceneMgr->getCamera(msg->getData()->first_attribute("Name")->value());
	auto tmpCameraCmp = boost::make_shared<EntityComponent::CameraComponent>();
	tmpCameraCmp->setCamera(cam);

	auto cameraCmpId = GET_COMP_REG(EntityComponent::CameraComponent).addComponent(tmpCameraCmp);
		//logger << boost::format("OgreSystem::createComponent entity %d created RenderComponent %d") % msg->getEntityId() % rndrCmpId;
	GET_ENTITY(msg->getEntityId())->addComponent<EntityComponent::CameraComponent>(cameraCmpId);

	auto wrldCmpId = ent->getComponent(EntityComponent::EntityWorldComponent::getItemTypeId());
	auto wrldCmp = GET_COMP_REG(EntityComponent::EntityWorldComponent).getComponentView(wrldCmpId);

	//If the entity has wrldCmpnt we need to use it's values for the camera.
	if(wrldCmp)
	{
		cam->setPosition(wrldCmp->getPosition());
	}

	return MsgSwitch::caseOperationRet();
}
void OgreSystem::createCamera(std::shared_ptr<rapidxml::xml_node<>> const val)
{
	LogMsg().Log("OgreSystem::createCamera");
	//BOOST_FOREACH(boost::property_tree::ptree::value_type &v,val)

	if(val)
	{

		auto camName = val->first_attribute("Name");

		std::string strCamName = "DefaultCamera";
		if(camName)
		{
			strCamName = boost::lexical_cast<std::string>(camName->value());
		}

		if(mSceneMgr->hasCamera(strCamName))
			mCamera = mSceneMgr->getCamera(strCamName);
		else
			mCamera = mSceneMgr->createCamera(strCamName);

		if(val->first_node("Position"))
			mCamera->setPosition(readVectorXml<Ogre::Vector3>(val->first_node("Position")));

		if(val->first_node("LookAt"))
			mCamera->lookAt(readVectorXml<Ogre::Vector3>(val->first_node("LookAt")));

		if(val->first_attribute("NearClipDistance"))
			mCamera->setNearClipDistance(boost::lexical_cast<int>(val->first_attribute("NearClipDistance")->value()));

		if(val->first_attribute("FarClipDistance"))
			mCamera->setFarClipDistance(boost::lexical_cast<int>(val->first_attribute("FarClipDistance")->value()));

		LogMsg() << boost::format("OgreSystem::createCamera created a camera %s at %d") % mCamera->getName() % mCamera->getPosition();


	}
	if(!mCamera)
	{
		LogMsg().Log("OgreSystem::createCamera Couldn't find a camera, defaulting to DefaultCamera");
		mCamera = mSceneMgr->createCamera("DefaultCamera");

		// Position it at 500 in Z direction
		mCamera->setPosition(Ogre::Vector3(0,200,200));
		// Look back along -Z
		mCamera->lookAt(Ogre::Vector3(0,0,0));
		mCamera->setNearClipDistance(5);

	}


	}

	void OgreSystem::createViewports(void)
	{
		// Create one viewport, entire window
		Ogre::Viewport* vp = mOgreComponent->mWindow->addViewport(mCamera);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(
				Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	}

	void OgreSystem::loadResources(void)
	{
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	void OgreSystem::createFrameListener(void)
	{

		//Register as a Window listener
		Ogre::WindowEventUtilities::addWindowEventListener(mOgreComponent->mWindow, this);
		mRoot->addFrameListener(this);
	}

	bool OgreSystem::setup(void)
	{

		bool carryOn = configure();
		if (!carryOn) return false;



		return true;
	}

	void OgreSystem::createPlanes(const std::shared_ptr<rapidxml::xml_node<> > val)
	{
		Ogre::Plane	plane(Ogre::Vector3::UNIT_Y, -10);
		Ogre::MeshManager::getSingleton().createPlane("plane",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,plane,
				15500,15500,20,20,true,1,5,5,Ogre::Vector3::UNIT_Z);


		Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity","plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
		ent->setMaterialName("Examples/Rockwall");

	}

	void OgreSystem::createLights(std::shared_ptr<rapidxml::xml_node<>> const val)
	{
		// Set ambient light
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
		Ogre::Light* light1 = mSceneMgr->createLight("light1");
		light1->setType(Ogre::Light::LT_POINT);
		light1->setPosition(0,20,0);
		light1->setDiffuseColour(1.0f,1.0f,1.0f);
	}

	void OgreSystem::createScene(std::shared_ptr<rapidxml::xml_node<>> const val)
	{


		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

		// World geometry
		//mSceneMgr->setWorldGeometry("terrain.cfg");

		Ogre::SceneNode* node = mSceneMgr->createSceneNode("planeLightSrc");
		mSceneMgr->getRootSceneNode()->addChild(node);



//		Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
//		node3->setScale(0.1f,0.1f,0.1f);
//		node3->setPosition(0,300,0);
		//node3->attachObject(mSceneMgr->getEntity("cube"));

		//The GUI system needs to be initiate in the same process data space as Ogre :(
		if(SystemsManager::getSingleton().getSystem<GuiSystem>())
			SystemsManager::getSingleton().getSystem<GuiSystem>()->startSystem();

	}

	void OgreSystem::setupResources(void)
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load(mResourcesCfg);

		// Go through all sections & settings in the file
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
						archName, typeName, secName);
			}
		}
	}

    DEFINE_CASE_IMP(OgreSystem::startSystem)
    {
		MsgSwitch::caseOperationRet	retVal;
		LogMsg().Log("OgreSystem::startSystem");
        std::string strPath = ConfigurationDirectory::getSingletonPtr()->get("GlobalConfigurations").get("ConfigurationPath");
        strPath += "/";
#ifdef _DEBUG
        mResourcesCfg = strPath + "resources_d.cfg";
        mPluginsCfg = strPath + "plugins_d.cfg";
#else
        mResourcesCfg = strPath + "resources.cfg";
        mPluginsCfg = strPath + "plugins.cfg";
#endif

		mRoot = new Ogre::Root(mPluginsCfg);

		setupResources();

		{

			if(mRenderInSameThread)
				run();
			else
			{

				mOgreSysRoutine =  boost::thread(&OgreSystem::run,this);
				mWaitForInit->wait();
			}

		}

		//retVal = boost::make_optional(MsgSwitch::caseOperationArg(new StartSystemMsg));
		return	msgSwitch::returnMoveToOutChannel;
	}

    DEFINE_CASE_IMP(OgreSystem::createComponent)
    {
		auto msg = CAST_MSG_TO_TYPE(CreateComponentMsg,val);
		auto ent = GET_ENTITY(msg->getEntityId());
		auto data = msg->getData();
		std::size_t wrldCmpId = 0;
		LogMsg	logger(0,0,false);

		if(!ent)
		{
			LogMsg() << boost::format("OgreSystem::createComponent CANNOT FIND ENTITY!!! %d") % msg->getEntityId();

		}
		else
		{
		//	boost::property_tree::write_xml("xmlDump.xml",msg->getData());


			wrldCmpId = ent->getComponent(EntityComponent::EntityWorldComponent::getItemTypeId());
			logger << boost::format("OgreSystem::createComponent entity %d using EntityWorldComponent %d") % msg->getEntityId() % wrldCmpId;
			auto wrldCmp = GET_COMP_REG(EntityComponent::EntityWorldComponent).getComponentView(wrldCmpId);

			//Basically there is no reason for RenderComponent AND EntityWorldComponent because the
			boost::shared_ptr<EntityComponent::RenderComponent>	tmpRndrCmp(new EntityComponent::RenderComponent);

			tmpRndrCmp->setSceneNode(std::shared_ptr<Ogre::SceneNode>(mSceneMgr->getRootSceneNode()->createChildSceneNode(wrldCmp->getPosition())));

			auto rndrCmpId = GET_COMP_REG(EntityComponent::RenderComponent).addComponent(tmpRndrCmp);
			//logger << boost::format("OgreSystem::createComponent entity %d created RenderComponent %d") % msg->getEntityId() % rndrCmpId;
			GET_ENTITY(msg->getEntityId())->addComponent<EntityComponent::RenderComponent>(rndrCmpId);

			std::string entName = "";
			std::string entMesh = "";

			if(data->first_attribute("Name"))
				entName = boost::lexical_cast<std::string>(data->first_attribute("Name")->value());

			if(data->first_attribute("Mesh"))
				entMesh = boost::lexical_cast<std::string>(data->first_attribute("Mesh")->value());

			Ogre::Entity* movableObj = nullptr;
			if(mSceneMgr->hasEntity(entName))
			{
				movableObj = mSceneMgr->getEntity(entName);
			}
			else
			{
				movableObj = mSceneMgr->createEntity(entName,entMesh);
			}

			tmpRndrCmp->getSceneNode()->setOrientation(Ogre::Euler(wrldCmp->getOrientation()));
			tmpRndrCmp->getSceneNode()->setScale(wrldCmp->getScale());
			tmpRndrCmp->getSceneNode()->attachObject(movableObj);
			movableObj->getUserObjectBindings().setUserAny("entityId",Ogre::AnyNumeric(msg->getEntityId()));

		}

		return	MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(OgreSystem::initSystem)
    {

		auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);

		auto initData = msg->getData()->first_node("OgreSystem");

		if(initData && initData->first_attribute("RenderFramesSeparately"))
			mRenderOne = boost::lexical_cast<bool>(initData->first_attribute("RenderFramesSeparately")->value());

		if(initData && initData->first_attribute("RenderInSameThread"))
			mRenderInSameThread = boost::lexical_cast<bool>(initData->first_attribute("RenderInSameThread")->value());

		if(initData && initData->first_attribute("MillisecondsBetweenFrames"))
			mMillisecondsBetweenFrames = boost::lexical_cast<int>(initData->first_attribute("MillisecondsBetweenFrames")->value());

		if(initData && initData->first_attribute("ShowConfiguration"))
			mShowConfiguration = boost::lexical_cast<bool>(initData->first_attribute("ShowConfiguration")->value());

		if(initData)
		{
			//std::cout << *(msg->getData());
			if(initData->first_node("Camera"))
			{
				mCamerasInformation = std::shared_ptr<rapidxml::xml_node<>>(msg->getData(),initData->first_node("Camera"));
			}
			if(initData->first_node("Lights"))
			{
				mLightsInformation = std::shared_ptr<rapidxml::xml_node<>>(msg->getData(),initData->first_node("Lights"));
			}
			if(initData->first_node("Meshes"))
			{
				mMeshesInformation = std::shared_ptr<rapidxml::xml_node<>>(msg->getData(),initData->first_node("Meshes"));
			}
			if(initData->first_node("Textures"))
			{
				mTexturesInformation = std::shared_ptr<rapidxml::xml_node<>>(msg->getData(),initData->first_node("Textures"));
			}
//			if(initData->first_node("Planes"))
//			{
//				mPlanesInformation = std::shared_ptr<rapidxml::xml_node<>>(msg->getData(),initData->first_node("Planes"));
//			}

		}
		return	MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(OgreSystem::shutDownSystem)
    {

		LogMsg().Log("OgreSystem::shutDownSystem");
		mOgreComponent->mWindow->destroy();
		Ogre::WindowEventUtilities::removeWindowEventListener(mOgreComponent->mWindow, this);
		windowClosed(mOgreComponent->mWindow);

		return	MsgSwitch::caseOperationRet();
	}

	bool OgreSystem::frameStarted(const Ogre::FrameEvent& evt) {
	return true;
	}

	bool OgreSystem::frameEnded(const Ogre::FrameEvent& evt) {
	return true;
	}

	bool OgreSystem::frameRenderingQueued(const Ogre::FrameEvent& evt) {
		if(mOgreComponent->mWindow->isClosed())
			return false;
		LogMsg	logger(0,0,false);

		sendMsg(MAKE_SHARED_MSG_TYPE<TickMsg>(evt));
        if(mDeltasVector.size())
            logger << boost::format("Frame: %d num of entities: %d time from last frame %f time from last event %f")
                        					% mFrameIdx % mDeltasVector.size() % evt.timeSinceLastFrame % evt.timeSinceLastEvent;
		++mFrameIdx;

//		EntityComponent::EntityRegistry::getSingletonPtr()->iterateRegistry([&](EntityComponent::EntityRegistry::registry_t::value_type const& val)
//				{
//					auto rndrCmpId = val.second->getComponent(EntityComponent::RenderComponent::getItemTypeId());
//					if(rndrCmpId)
//					{
//						auto rndrCmp = GET_COMP_REG(EntityComponent::RenderComponent).getComponent(rndrCmpId);
//						if(rndrCmp)
//						{
//							auto sceneNode = rndrCmp->getSceneNode();
//							if(sceneNode->getAttachedObject(0))
//							{
//								auto animSet = static_cast<Ogre::Entity*>(sceneNode->getAttachedObject(0))->getAllAnimationStates();
//								if (animSet)
//								{
//									auto enabledAnimIter = animSet->getEnabledAnimationStateIterator();
//									while(enabledAnimIter.hasMoreElements())
//									{
//										enabledAnimIter.getNext()->addTime(evt.timeSinceLastFrame);
//									}
//								}
//							}
//						}
//					}
//				});
		decltype(mDeltasVector) copy;
		{
			boost::lock_guard<boost::mutex> lock(mutex);

			copy = mDeltasVector;
			mDeltasVector.clear();
		}
		for(auto iDx = copy.cbegin();iDx != copy.cend();++iDx)
		{
			auto entityId = iDx->first;
			auto msg = iDx->second;

			//BOOST_ASSERT_MSG(msg,"OgreSystem::PositionChanged	msg is NULL");

			auto ent = GET_ENTITY(entityId);

			std::size_t rndrCmpId = 0;
			if(!ent)
			{
				logger << boost::format("CANNOT FIND ENTITY!!!: %d") % entityId;
			}
			else
			{
				rndrCmpId = ent->getComponent(EntityComponent::RenderComponent::getItemTypeId());
				if(rndrCmpId){
					auto rndrCmp = GET_COMP_REG(EntityComponent::RenderComponent).getComponent(rndrCmpId);
					if(rndrCmp)
					{

						auto sceneNode = rndrCmp->getSceneNode();

						auto pos = msg->getPosition();
						auto rot = msg->getOrientation();
						auto ogrePos = Ogre::Vector3(pos[0],pos[1],pos[2]);
						auto ogreRot = Ogre::Quaternion(rot[0],rot[1],rot[2],rot[3]);
						//logger << boost::format("Ogre: %d: %d") % entityId % ogrePos;
						if(sceneNode)
						{
							sceneNode->setPosition(ogrePos);
							sceneNode->setOrientation(ogreRot);
						}
						else
							LogMsg() << boost::format("No SceneNode entity: %d") % entityId;

						logger << boost::format("OgreSystem::PositionChanged finished entity: %d") % entityId;

					}
				}
				auto cameraCmpId = ent->getComponent<EntityComponent::CameraComponent>();
				if(cameraCmpId){
					auto cameraCmp = GET_COMP_REG(EntityComponent::CameraComponent).getComponent(cameraCmpId);
					if(cameraCmp)
					{
						auto pos = msg->getPosition();
						auto rot = msg->getOrientation();
						auto ogrePos = Ogre::Vector3(pos[0],pos[1],pos[2]);
						auto ogreRot = Ogre::Quaternion(rot[0],rot[1],rot[2],rot[3]);
						cameraCmp->getCamera()->setPosition(ogrePos);
						//cameraCmp->getCamera()->setOrientation(ogreRot);
					}
				}
			}
		}


		return true;
	}

	void OgreSystem::run() {

		setup();

		chooseSceneManager();


		createCamera(mCamerasInformation);

		createViewports();


		createLights(mLightsInformation);

		createTextures(mTexturesInformation);


		// Set default mipmap level (NB some APIs ignore this)
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);


		// Create any resource listeners (for loading screens)
		// createResourceListener();
		// Load resources

		loadResources();

		createMeshs(mMeshesInformation);
		// Create the scene

		createScene(mSceneInformation);
		createPlanes(mPlanesInformation);

		createFrameListener();



		LogMsg().Log("Start Rendering");
		mWaitForInit->wait();
		if(mRenderOne)
		{
			auto ticks = boost::posix_time::milliseconds(50);
			mRoot->getRenderSystem()->_initRenderTargets();
			mRoot->clearEventTimes();
			while(true)
			{
				Ogre::WindowEventUtilities::messagePump();
				if(!mRoot->renderOneFrame(ticks.ticks_per_second()))
				{
					LogMsg().Log("Failed rendering");
					break;
				}
				boost::this_thread::sleep(ticks);
			}
		}
		else
		{
			mRoot->startRendering();
		}
		LogMsg().Log("Stopped Rendering");
		shutDownSystem();

	}

	void OgreSystem::createMeshs(std::shared_ptr<rapidxml::xml_node<>> const val) {
	//	mEnt = mSceneMgr->createEntity("sphere","sphere.mesh");
	//	mSceneMgr->createEntity("cube","cube.mesh");
	//	mSceneMgr->createEntity("robot","robot.mesh");
	//	mSceneMgr->createEntity("barrel","Barrel.mesh");
		for(auto iter = val->first_node(); iter; iter = iter->next_sibling())
		{
//			if(v.second.find("Load") != v.second.not_found())
//			{
//				//Need to finish this part
//				//Problems I've found:
//				//	*	the .obj format is not that similar to the manual object's api
//				//	*	I need to change the manualObject into an entity
//				//	*	Only indexed geometries can be cpnverted into meshes
//				//	*	Blender exports the entire scene into obj file but I need it in separated files (per mesh)
//
//				auto  meshData = v.second.get<std::string>("Load");
//				auto mesh = createManualObjectFromFile(meshData);
	//			if(mesh != nullptr)
	//				mesh->c
//			}
//			else
			{
				std::string entName;
				std::string entMesh;

				if(iter->first_attribute("Name"))
					entName = boost::lexical_cast<std::string>(iter->first_attribute("Name")->value());

				if(iter->first_attribute("Mesh"))
					entMesh = boost::lexical_cast<std::string>(iter->first_attribute("Mesh")->value());

				mSceneMgr->createEntity(entName,entMesh);

			}

		}

	}
	void OgreSystem::createTextures(std::shared_ptr<rapidxml::xml_node<>> const val) {
	}

    DEFINE_CASE_IMP(OgreSystem::PositionChanged)
    {
		auto msg = CAST_MSG_TO_TYPE(PositionChangedMsg,val);
		boost::lock_guard<boost::mutex> lock(mutex);
		mDeltasVector.insert(std::make_pair(msg->getEntityId(),msg));
		return MsgSwitch::caseOperationRet();

	}

    DEFINE_CASE_IMP(OgreSystem::moveCamera)
    {

		auto msg = CAST_MSG_TO_TYPE(MoveCameraMsg,val);

		auto vec = msg->getData();
		auto camId = msg->getCameraID();
		Ogre::Camera* cam = nullptr;

		if(mSceneMgr->hasCamera(camId))	//and if camId is not empty
			cam = mSceneMgr->getCamera(camId);
		else
			cam = mSceneMgr->getCamera("DefaultCamera");	//Maybe later it should be getActiveCamera and not default


		if(cam) cam->setPosition(cam->getPosition() + vec);
//		Systems::sendMsg(boost::make_shared<PositionChangedMsg>(*entID,trans.getOrigin(),trans.getRotation()));
//

		LogMsg().Log("OgreSystem::moveCamera");
		return MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(OgreSystem::startAnimation)
	{
		auto msg = CAST_MSG_TO_TYPE(StartAnimationMsg,val);
		auto ent = GET_ENTITY(msg->getEntityID());
		auto rndrCmpID = ent->getComponent<EntityComponent::RenderComponent>();
		if(rndrCmpID)
		{
			auto rndrCmp = GET_COMP_REG(EntityComponent::RenderComponent).getComponent(rndrCmpID);
			auto ogreEntity = static_cast<Ogre::Entity*>(rndrCmp->getSceneNode()->getAttachedObject(0));
			auto anim = ogreEntity->getAnimationState(msg->getData());
			anim->setEnabled(true);
			anim->setLoop(true);
		}

		return MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(OgreSystem::castRay)
	{
		//First convert the msg into a castrayMsg
		//ask the gui system if the position belongs to her
		//if Gui::hitTest true than send the MouseKeyPressedMsg to GuiSystem
		//else
		//send the msg castray function to graphicsystem

		auto msg = CAST_MSG_TO_TYPE(CastRayMsg,val);
		auto	&mousePos = msg->getData();
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int colourDepth = 0;
		int left = 0;
		int top = 0;


		mOgreComponent->mWindow->getMetrics(width,height,colourDepth,left,top);
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.x/float(width), mousePos.y/float(height));

        out.addMsg(VectorMsg(mouseRay.getOrigin()).createSharedPtr());

        auto raySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

		raySceneQuery->setRay(mouseRay);
		raySceneQuery->setSortByDistance(true);

        LogMsg() << boost::format("OgreSystem::castRay ray origin: %1% direction: %2% ") % mouseRay.getOrigin() % mouseRay.getDirection();

		Ogre::RaySceneQueryResult &result = raySceneQuery->execute();
		for(auto iter = result.begin();iter != result.end();++iter)
		{
			if (iter->movable)
			{
				if(iter->movable->getName().substr(0, 5) != "tile[")
				{
					auto mTmpSceneNode = iter->movable->getParentSceneNode();
					if (mTmpSceneNode)
					{
						mTmpSceneNode->showBoundingBox(true);
						auto tmp = iter->movable->getUserObjectBindings().getUserAny("entityId");
						if(!tmp.isEmpty())
						{
							auto entityId = Ogre::any_cast<std::size_t>(tmp);
							LogMsg() << boost::format("OgreSystem::castRay pointed to %s id: %d") % iter->movable->getName() % entityId;
                            out.addMsg(FocusEntity(entityId).createSharedPtr());
                        }


					}
				}
				else
					LogMsg() << boost::format("OgreSystem::castRay pointed to %s") % iter->movable->getName();
			}
		}
		return MsgSwitch::caseOperationRet();
	}

}


/* namespace Systems */


