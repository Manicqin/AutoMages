/*
 * ControllerMessages.h
 *
 *  Created on: Sep 28, 2012
 *      Author: manicqin
 */

#ifndef CONTROLLERMESSAGES_H_
#define CONTROLLERMESSAGES_H_

#include "Messages.h"
#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

DataMessageDeclare2(ControllerKeyMsg,OIS::KeyEvent,bool,Down);
DataMessageDeclare(ControllerMouseMoveMsg,OIS::MouseEvent);
DataMessageDeclare3(ControllerMousePressedMsg,OIS::MouseState,OIS::MouseButtonID,Bid,bool,Down);

DataMessageDeclare4(ControllerBufferedKeyMsg,OIS::KeyCode,bool,Down,float,Interval,float,Overall);
DataMessageDeclare(ControllerBufferedMouseMoveMsg,OIS::MouseState);
DataMessageDeclare4(ControllerBufferedMousePressedMsg,OIS::MouseButtonID,bool,Down,float,Interval,float,Overall);

EmptyMessageDeclare(CaptureMsg);

DataMessageDeclare4(ControllerSubscribedKeyMsg,std::size_t,bool,Down,float,Interval,float,Overall);
//DataMessageDeclare(ControllerSubscribedMouseMoveMsg,OIS::MouseState);
//DataMessageDeclare4(ControllerSubsribedMousePressedMsg,OIS::MouseButtonID,bool,Down,float,Interval,float,Overall);

typedef std::vector<OIS::KeyCode>	keySet;
typedef std::vector<OIS::MouseButtonID>	mouseSet;

struct	KeySubscribtion
{
	bool isDown = false;
	bool isUp = false;
	float downInterval = 0.0f;
	keySet	keys;
	mouseSet mouseKeys;
	std::size_t		subscriber;
};

DataMessageDeclare2(SubscribeToInputMsg,std::size_t,KeySubscribtion,SubscribtionData);

#endif /* CONTROLLERMESSAGES_H_ */


/*
 * 	We need
 *
 * 	Subscription Key
 *
 * 	KeyCode
 * 	KeyCode array
 *
 * 	Is Down?
 * 	Is Up?
 * 	Is Down for x time
 *
 *	Would I need to support key is not down for x interval? sounds spammy...
 *	Is it smart to always use key set? even id we have only one key?
 */
