/*
 * ScriptComponent.h
 *
 *  Created on: Jul 26, 2013
 *      Author: manicqin
 */

#ifndef SCRIPTCOMPONENT_H_
#define SCRIPTCOMPONENT_H_
#include "Component.h"
#include <boost/python/object.hpp>

namespace EntityComponent{

ComponentDeclare1(ScriptComponent,PyObject*,Data);
}

#endif /* SCRIPTCOMPONENT_H_ */
