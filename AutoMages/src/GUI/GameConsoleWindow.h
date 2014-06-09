/*
 * GameConsoleWindow.h
 *
 *  Created on: Jun 21, 2012
 *      Author: manicqin
 */

#ifndef GAMECONSOLEWINDOW_H_
#define GAMECONSOLEWINDOW_H_



#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <CEGUIcolour.h>

#include <boost/program_options.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>

#include "../EntityComponents/Messages.h"
#include "../Systems/SystemBase.h"
class GameConsoleWindow {
public:
       GameConsoleWindow();                   // Constructor
       void setVisible(bool visible);         // Hide or show the console
       bool isVisible();                      // return true if console is visible, false if is hidden
       bool InjectText(const CEGUI::EventArgs &e);

    private:
       void CreateCEGUIWindow();                                  // The function which will load in the CEGUI Window and register event handlers
       bool Handle_KeyUp(const CEGUI::EventArgs &e);
       bool Handle_KeyDown(const CEGUI::EventArgs &e);

       void RegisterHandlers();                                   // Register our handler functions
       bool Handle_TextSubmitted(const CEGUI::EventArgs &e);      // Handle when we press Enter after typing
       bool Handle_SendButtonPressed(const CEGUI::EventArgs &e);  // Handle when we press the Send button
       void ParseText(CEGUI::String inMsg);                       // Parse the text the user submitted.
       void OutputText(CEGUI::String const& inMsg,  CEGUI::colour colour = CEGUI::colour( 0xFFFFFFFF)); //   with a white color default
       //void OutputText(std::string const& inMsg);

       void ExecuteMacro(std::string strFileName);
       CEGUI::Window *m_ConsoleWindow;                            // This will be a pointer to the ConsoleRoot window.
       CEGUI::String sNamePrefix;                                  // This will be the prefix name we give the layout
       CEGUI::Editbox* mEditBox;
       CEGUI::Listbox* mListBox;
       static int iInstanceNumber;                                 // This will be the instance number for this class.
       bool m_bConsole;

       boost::program_options::options_description mDesc;
       //PythonBindings	mPythonBinding;

       boost::circular_buffer<std::string>	mLastLines;
       std::vector<std::string> mRecordedLines;
       const int mNumberOfLinesToSave;

       //boost::thread	pythonBindingsWorker;
       bool		mIsRecordind;

       bool		mIsCtrlPressed;
       bool		mIsShiftPressed;
       uint 	mEventHandled;

       MsgSwitch	mOperations;
};


#endif /* GAMECONSOLEWINDOW_H_ */
