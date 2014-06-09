/*
 * EntityRegister.cpp
 *
 *  Created on: Jun 10, 2012
 *      Author: manicqin
 */

#include "EntityRegister.h"

template<> EntityRegister* Ogre::Singleton<EntityRegister>::msSingleton = nullptr;

EntityRegister::EntityRegister():mFreeId(std::numeric_limits<entityId_t>::min()) {
	// TODO Auto-generated constructor stub

}

EntityRegister::~EntityRegister() {
	// TODO Auto-generated destructor stub
}

//EntityRegister* EntityRegister::getSingletonPtr(void)
//{
//	return ms_Singleton;
//}
//EntityRegister& EntityRegister::getSingleton(void)
//{
//	assert( ms_Singleton );  return ( *ms_Singleton );
//}
//

