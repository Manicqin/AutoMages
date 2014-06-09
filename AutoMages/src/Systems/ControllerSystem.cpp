/*
 * ControllerSystem.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: manicqin
 */

#include "ControllerSystem.h"
#include "LoggingSystem.h"

#include "../EntityComponents/SystemsMessages.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/OgreMiscComponent.h"
#include "../EntityComponents/AllMessages.h"
#include "../Systems/OgreSystem.h"

#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace Systems {

	/*
	 * Keyboard layout:
	 *
	 *	Movement:
	 *		Arraws \ WSAD \ Page up \ Page Down \ etc
	 *
	 *	1 key Actions:
	 *		Space \ Enter \ Tab \ 'a' - 'z' \ Function Keys \ ...
	 *
	 *	2 key Actions:
	 *		Ctrl \ Alt \ Shift
	 *
	 *  A consumer subscribes to  a "Domain"
	 *  There can be more than one subscriber to a Domain.
	 *
	 *  We add OgreSys as the subscriber of Movement keys
	 *  There are 8 keys for movement (4 basic + 4 extra (7+home / 9+PgUp))
	 *  But there could be more... so we need a dynamic way
	 *  how about:
	 *  	There's ONE python file that is used by the controllerSystem and the subscribers
	 *  	The controller reads the domains out of it
	 *  	The subscribing systems use it to translate the key and as a callback... need to think it through
	 *  	for starters I will register movement keys and rotate around
	 *
	 *
	 *
	 */
	//Adjust mouse clipping area
	void ControllerSystem::windowResized(Ogre::RenderWindow* rw)
	{
		unsigned int width, height, depth;
		int left, top;
		rw->getMetrics(width, height, depth, left, top);

		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}
	ControllerSystem::ControllerSystem():EnumedSystem<ControllerSystem>("ControllerSystem") {
	/*	getOperations()
				.CASE_WRAP_SCRIPT_FUNC(CaptureMsg)
				.CASE_WRAP_SCRIPT_FUNC(ControllerMousePressedMsg)
				.CASE_WRAP_SCRIPT_FUNC(ControllerKeyMsg);
				.CASE_WRAP_SCRIPT_FUNC(SubscribeToInputMsg);*/

	}

	ControllerSystem::~ControllerSystem() {
		// TODO Auto-generated destructor stub
	}


	bool ControllerSystem::keyPressed(const OIS::KeyEvent& ke) {
		auto msg = boost::make_shared<ControllerKeyMsg>(ke);
		msg->setDown(true);

		ChannelHandler channel2(ChannelHandler::createChannelHandler<ThreadSafeChannel>("ControllerSystem::KeyPressed"));


			// ..... We need to pass the key to the Python interpreter
			// We can hold an object class that is already the pre-interpret object
			// but we can only attack Python from one thread :(
			// Script components? hold the python object each system needs
			// 		how do we do the auto complete?!?
			// 		ok :( we first set the wordToComplete variable and then we execute the script
			// 		2 things, we first need msg aggregator, so things like that will happen in sequence!
			// 		second maybe we need to find a better way to do such things.
			// will the script comp communicate with the script sys? that's not the way we work....
			//

	//		Systems::sendMsg(ChanneledMessages::create(channel2,
	//				MAKE_SHARED_MSG_TYPE<ScriptExecuteFile>("Controls.py")
	//								));

		Systems::sendMsg(msg);
			//getChannelOut().addMsg();
		return true;
	}

	bool ControllerSystem::keyReleased(const OIS::KeyEvent& ke)
    {
		auto msg = boost::make_shared<ControllerKeyMsg>(ke);
		msg->setDown(false);
		Systems::sendMsg(msg);
		return true;
	}

	bool ControllerSystem::mouseMoved(const OIS::MouseEvent& ke)
	{
		auto msg = boost::make_shared<ControllerMouseMoveMsg>(ke);
		Systems::sendMsg(msg);
		return true;
	}

	bool ControllerSystem::mousePressed(const OIS::MouseEvent& me,
			OIS::MouseButtonID mbid) {
        LogMsg().Log("ControllerSystem::mousePressed");
		auto msg = boost::make_shared<ControllerMousePressedMsg>(me.state);
		msg->setBid(mbid);
		msg->setDown(true);

		Systems::sendMsg(msg);
		return true;
	}

	bool ControllerSystem::mouseReleased(const OIS::MouseEvent& me,
			OIS::MouseButtonID mbid) {
        LogMsg().Log("ControllerSystem::mouseReleased");
		auto msg = boost::make_shared<ControllerMousePressedMsg>(me.state);
		msg->setBid(mbid);
		msg->setDown(false);

		Systems::sendMsg(msg);
		return true;
	}

    DEFINE_CASE_IMP(ControllerSystem::startSystem)
	{

		auto tmp = SystemsManager::getSingleton().getSystem<OgreSystem>();
		auto miscStorage = GET_COMP_REG(EntityComponent::MiscComponent).getComponent(tmp->getOgreComponentId());
		auto storage = boost::dynamic_pointer_cast<EntityComponent::OgreMiscComponent>(miscStorage);

		if(!storage->mWindow)
		{
			LogMsg().Log("ControllerSystem::startSystem Couldn't find windows, reusing msg");
		//	getChannelIn().addMsg(val);
		}
		else
		{
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;

			LogMsg().Log("ControllerSystem::startSystem found window");
			storage->mWindow->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
			pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
			pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
			pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
			pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
			mInputManager = OIS::InputManager::createInputSystem( pl );

			mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
			mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

			mMouse->setEventCallback(this);
			mKeyboard->setEventCallback(this);

			//Set initial mouse clipping size
			windowResized(storage->mWindow);
			mControllerSysRoutine =  boost::thread(&ControllerSystem::run,this);
		}
		return	MsgSwitch::caseOperationRet();

	}

    DEFINE_CASE_IMP(ControllerSystem::createComponent)
    {
		return	MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(ControllerSystem::initSystem)
    {
		return	MsgSwitch::caseOperationRet();
	}

    DEFINE_CASE_IMP(ControllerSystem::shutDownSystem)
    {
		return	MsgSwitch::caseOperationRet();
	}
	void ControllerSystem::run() {

		CaptureMsg	tmpMsg;
		auto captureMsg = tmpMsg.createSharedPtr();
		auto tick =  boost::posix_time::second_clock::local_time();
		while(true){
			mMouse->capture();
			mKeyboard->capture();

			auto id = GET_ENTITY(getSystemEntityId())->getComponent<EntityComponent::ScriptComponent>();
			auto now =  boost::posix_time::microsec_clock::local_time();
			auto diff = now - tick;
			if(GET_COMP_REG(EntityComponent::ScriptComponent).getComponent(id) 
					&& diff.total_milliseconds() > 100 )
			{
				sendMsg(captureMsg);
				tick =  boost::posix_time::microsec_clock::local_time();
			//	LogMsg().Log(boost::posix_time::to_simple_string(tick));

			}
//			boost::this_thread::yield();
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}



}


/* namespace Systems */


