/*
 * CEGuiUtils.h
 *
 *  Created on: Nov 20, 2012
 *      Author: manicqin
 */

#ifndef CEGUIUTILS_H_
#define CEGUIUTILS_H_
#include <OIS/OIS.h>
#include <CEGUI.h>
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




#endif /* CEGUIUTILS_H_ */
