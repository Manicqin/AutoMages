/*
 * EntitySystem.h
 *
 *  Created on: Jul 20, 2013
 *      Author: manicqin
 */

#ifndef ENTITYSYSTEM_H_
#define ENTITYSYSTEM_H_

#include "SystemBase.h"
#include "ScriptManager.h"
#include "LoggingSystem.h"

#include "../EntityComponents/Entity.h"
#include "../EntityComponents/Component.h"
#include "../EntityComponents/ScriptComponent.h"
namespace Systems {









class EntitySystem: public Systems::EnumedSystem<EntitySystem> {
public:
	EntitySystem();
	virtual ~EntitySystem();

    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(createEntity);

	//MsgSwitch::caseOperationRet createComponent(MsgSwitch::caseOperationArg const& val = CASE_OPERATION_ARG_DEF_VAL);
private:
	bool mWaitOnEntityBuild;
};

} /* namespace Systems */
#endif /* ENTITYSYSTEM_H_ */
