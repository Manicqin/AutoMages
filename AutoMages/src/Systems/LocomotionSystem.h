/*
 * LocomotionSystem.h
 *
 *  Created on: Oct 8, 2012
 *      Author: manicqin
 */

#ifndef LOCOMOTIONSYSTEM_H_
#define LOCOMOTIONSYSTEM_H_

#include "SystemBase.h"
#include "Channel.h"
namespace Systems {

class LocomotionSystem : public EnumedSystem <LocomotionSystem> {
public:
	LocomotionSystem();
	virtual ~LocomotionSystem();

private:
    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);
};

} /* namespace EntityComponent */
#endif /* LOCOMOTIONSYSTEM_H_ */
