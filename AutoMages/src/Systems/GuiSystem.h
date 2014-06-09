/*
 * GuiSystem.h
 *
 *  Created on: Nov 20, 2012
 *      Author: manicqin
 */

#ifndef GUISYSTEM_H_
#define GUISYSTEM_H_

#include "SystemBase.h"
#include "../EntityComponents/GuiComponent.h"


namespace Systems {

class GuiSystem : public EnumedSystem <GuiSystem>{
public:
	GuiSystem():EnumedSystem <GuiSystem>("GuiSystem"){}

	virtual ~GuiSystem(){}


};

} /* namespace Systems */
#endif /* GUISYSTEM_H_ */
