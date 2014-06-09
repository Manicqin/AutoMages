/*
 * GuiComponent.h
 *
 *  Created on: Nov 20, 2012
 *      Author: manicqin
 */

#ifndef GUICOMPONENT_H_
#define GUICOMPONENT_H_

#include "Component.h"
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

namespace EntityComponent{
class GuiComponent : public MiscComponent
{
public:
	GuiComponent();
	virtual ~GuiComponent();
	CEGUI::Renderer*	mRenderer;
};
}
#endif /* GUICOMPONENT_H_ */
