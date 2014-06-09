/*
 * PyGuiSystem.h
 *
 *  Created on: Dec 8, 2013
 *      Author: manicqin
 */

#ifndef PYGUISYSTEM_H_
#define PYGUISYSTEM_H_

#include "GuiSystem.h"

namespace Systems {

class PyGuiSystem: public GuiSystem {
public:
	PyGuiSystem();
	virtual ~PyGuiSystem();

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

};

} /* namespace Systems */

#endif /* PYGUISYSTEM_H_ */
