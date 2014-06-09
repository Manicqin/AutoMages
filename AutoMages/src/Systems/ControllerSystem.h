/*
 * ControllerSystem.h
 *
 *  Created on: Oct 28, 2012
 *      Author: manicqin
 */

#ifndef CONTROLLERSYSTEM_H_
#define CONTROLLERSYSTEM_H_

#include "SystemBase.h"
#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

namespace Systems {

class ControllerSystem : public EnumedSystem<ControllerSystem> ,public OIS::KeyListener, public OIS::MouseListener{
public:
	ControllerSystem();
	virtual ~ControllerSystem();

private:
	void run();
	virtual bool keyPressed(const OIS::KeyEvent& ke);
	virtual bool keyReleased(const OIS::KeyEvent& ke);

	virtual bool mouseMoved(const OIS::MouseEvent& ke);

	virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID mbid);
	virtual bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID mbid);

	void windowResized(Ogre::RenderWindow* rw);

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

	//MsgSwitch::caseOperationRet sendCapture(MsgSwitch::caseOperationArg val);


    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

    boost::thread	mControllerSysRoutine;
};

} /* namespace Systems */
#endif /* CONTROLLERSYSTEM_H_ */
