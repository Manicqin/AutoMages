/*
 * CEGuiSystem.h
 *
 *  Created on: Dec 8, 2013
 *      Author: manicqin
 */

#ifndef CEGUISYSTEM_H_
#define CEGUISYSTEM_H_

#include "GuiSystem.h"
#include "../GUI/GameConsoleWindow.h"
namespace Systems {

class CEGuiSystem: public Systems::GuiSystem {
public:
	CEGuiSystem();
	virtual ~CEGuiSystem();

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

    DEFINE_CASE_PROTO(injectKeyboard);
    DEFINE_CASE_PROTO(injectMouse);

	bool hitTest(const int X, const int Y);
private:

	CEGUI::Renderer*	mRenderer;
	GameConsoleWindow*	mConsole;

	bool	mGameConsoleVisible;
};

} /* namespace Systems */

#endif /* CEGUISYSTEM_H_ */
