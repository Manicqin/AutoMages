/*
 * OgreMiscComponent.h
 *
 *  Created on: Oct 29, 2012
 *      Author: manicqin
 */

#ifndef OGREMISCCOMPONENT_H_
#define OGREMISCCOMPONENT_H_

#include "Component.h"

#include <Ogre.h>
namespace EntityComponent {

class OgreMiscComponent : public MiscComponent {
public:
	OgreMiscComponent();
	virtual ~OgreMiscComponent();


	Ogre::RenderWindow* mWindow;


};

} /* namespace Systems */
#endif /* OGREMISCCOMPONENT_H_ */
