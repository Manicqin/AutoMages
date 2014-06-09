/*
 * OgreSystem.h
 *
 *  Created on: Oct 27, 2012
 *      Author: manicqin
 */

#ifndef OGRESYSTEM_H_
#define OGRESYSTEM_H_

#include "SystemBase.h"

#include <map>
#include <memory>

#include <OgreString.h>
#include <OgreLogManager.h>
#include <OgreFrameListener.h>
#include <OgreWindowEventUtilities.h>
//#include <SdkCameraMan.h>

#include <boost/thread.hpp>

#include "ScriptManager.h"

#include "../EntityComponents/OgreMiscComponent.h"
#include "../EntityComponents/PhysicsMessages.h"
//#include <SdkTrays.h>
//#include <SdkCameraMan.h>
namespace Systems {

class OgreSystem :  public EnumedSystem <OgreSystem>,
					public Ogre::FrameListener, public Ogre::WindowEventListener

{
public:
	OgreSystem();

	virtual ~OgreSystem();

//	virtual void operator ()(const std::shared_ptr<Messages>& incomingMsg,
//				ChannelHandler outChannel);


	std::size_t getOgreComponentId() const {
		return mOgreComponentId;
	}

	void setOgreComponentId(std::size_t ogreComponentId) {
		mOgreComponentId = ogreComponentId;
	}

	Ogre::SceneManager* getSceneMgr() const {
		return mSceneMgr;
	}

private:

    DEFINE_CASE_PROTO(PositionChanged);

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);
    DEFINE_CASE_PROTO(createComponent);

    DEFINE_CASE_PROTO(moveCamera);

    DEFINE_CASE_PROTO(castRay);
    DEFINE_CASE_PROTO(startAnimation);

    DEFINE_CASE_PROTO(createCameraComponent);
	void run();

	void createMeshs(std::shared_ptr<rapidxml::xml_node<>> const val);
	void createCamera(std::shared_ptr<rapidxml::xml_node<>> const val);
	void createLights(std::shared_ptr<rapidxml::xml_node<>> const val);
	void createTextures(std::shared_ptr<rapidxml::xml_node<>> const val);
	void createPlanes(std::shared_ptr<rapidxml::xml_node<>> const val);
	void createScene(std::shared_ptr<rapidxml::xml_node<>> const val);

	void createFrameListener(void);

	void loadResources(void);
	void createViewports(void);

	void chooseSceneManager(void);
	bool configure(void);
	bool setup(void);
	void setupResources(void);

	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
	Ogre::Root *mRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;

	std::size_t	mFrameIdx;
	//Shared component will be used by OIS and GUI
    //Hold the window information and alike
    std::size_t		mOgreComponentId;
    boost::shared_ptr<EntityComponent::OgreMiscComponent>	mOgreComponent;
    //std::shared_ptr<EntityComponent::ScriptComponent>	mScriptComponent;

    Ogre::LogManager	mLogManager;
    boost::thread	mOgreSysRoutine;

    bool	mShowConfiguration;
    bool	mRenderInSameThread;
    bool	mRenderOne;
    int		mMillisecondsBetweenFrames;
    boost::barrier *mWaitForInit;

    std::shared_ptr<rapidxml::xml_node<>>	mSceneInformation;
    std::shared_ptr<rapidxml::xml_node<>>	mCamerasInformation;
    std::shared_ptr<rapidxml::xml_node<>>	mLightsInformation;
    std::shared_ptr<rapidxml::xml_node<>>	mMeshesInformation;
    std::shared_ptr<rapidxml::xml_node<>>	mTexturesInformation;
    std::shared_ptr<rapidxml::xml_node<>>	mPlanesInformation;

    boost::mutex	mutex;
    std::map<std::size_t,boost::shared_ptr<PositionChangedMsg> > mDeltasVector;
};

} /* namespace Systems */
#endif /* OGRESYSTEM_H_ */
