/*
 * CEGuiSystem.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: manicqin
 */

#include "CEGuiSystem.h"
#include "SystemsManager.h"
#include "OgreSystem.h"
#include "LoggingSystem.h"
#include "../EntityComponents/ControllerMessages.h"
#include "../EntityComponents/SystemsMessages.h"
#include "../Utilities/CEGuiUtils.h"

#include "../GUI/GameConsoleWindow.h"

#include <boost/property_tree/xml_parser.hpp>

#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <OIS/OIS.h>
namespace Systems {


CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;

    case OIS::MB_Right:
        return CEGUI::RightButton;

    case OIS::MB_Middle:
        return CEGUI::MiddleButton;

    default:
        return CEGUI::LeftButton;
    }
}

CEGuiSystem::CEGuiSystem():	mGameConsoleVisible(false) {

    getOperations().case_<ControllerKeyMsg>(std::bind(&CEGuiSystem::injectKeyboard,this,std::placeholders::_1,std::placeholders::_2))
            .case_<ControllerMousePressedMsg>(std::bind(&CEGuiSystem::injectMouse,this,std::placeholders::_1,std::placeholders::_2));
}

CEGuiSystem::~CEGuiSystem() {
	// TODO Auto-generated destructor stub
}

DEFINE_CASE_IMP(CEGuiSystem::startSystem)
{

	LogMsg().Log("CEGuiSystem::startSystem");

	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultFont( "DejaVuSans-10" );
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

	mConsole	= new GameConsoleWindow;
	mConsole->setVisible(mGameConsoleVisible);

	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(CEGuiSystem::createComponent)
{


	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(CEGuiSystem::initSystem)
{

	auto msg = CAST_MSG_TO_TYPE(InitSystemMsg,val);
	auto initData = msg->getData()->first_node("GuiSystem");
	auto data = initData->first_node("GameConsoleVisible");
	if(data)
		mGameConsoleVisible = boost::lexical_cast<bool>(data->value());

	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(CEGuiSystem::shutDownSystem)
{

	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(CEGuiSystem::injectKeyboard)
{

	auto msg = CAST_MSG_TO_TYPE(ControllerKeyMsg,val);

	if(msg->getDown())
	{
		CEGUI::System::getSingleton().injectKeyDown(msg->getData().key);
		CEGUI::System::getSingleton().injectChar(msg->getData().text);
	}
	else
	{
		CEGUI::System::getSingleton().injectKeyUp(msg->getData().key );
	}


	return MsgSwitch::caseOperationRet();
}

DEFINE_CASE_IMP(CEGuiSystem::injectMouse)
{

	if(val->getMessageType() == ControllerMouseMoveMsg::getItemTypeId())
	{
		auto msg = CAST_MSG_TO_TYPE(ControllerMouseMoveMsg,val);
		auto arg = msg->getData();
		CEGUI::System &sys = CEGUI::System::getSingleton();
				sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
				// Scroll wheel.
				if (arg.state.Z.rel)
					sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

	}
	else if(val->getMessageType() == ControllerMousePressedMsg::getItemTypeId())
	{
		auto msg = CAST_MSG_TO_TYPE(ControllerMousePressedMsg,val);
		auto id = msg->getBid();
		if(msg->getDown())
			CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
		else
			CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	}
	return MsgSwitch::caseOperationRet();
}
bool CEGuiSystem::hitTest(const int X, const int Y) {
	return false;
}

} /* namespace Systems */
