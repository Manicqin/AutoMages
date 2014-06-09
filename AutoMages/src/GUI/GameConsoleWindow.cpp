/*
 * GameConsoleWindow.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: manicqin
 */

#include "GameConsoleWindow.h"

#include <CEGUI.h>
#include <CEGUIWindow.h>
#include <CEGUIWindowManager.h>

#include "FormattedListboxTextItem.h"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "../Systems/ScriptManager.h"
#include "../Systems/SystemsManager.h"

#include "../EntityComponents/AllMessages.h"
int GameConsoleWindow::iInstanceNumber;            // Don't forget this declaration

using namespace boost::program_options;

GameConsoleWindow::GameConsoleWindow():mNumberOfLinesToSave(10),mIsRecordind(false)
{
	m_ConsoleWindow = NULL;       // Always good practice to initialize a pointer to a NULL value, helps when switching to Release Builds.
	iInstanceNumber = 0;
	sNamePrefix = "";
	CreateCEGUIWindow();
	setVisible(false);
	m_bConsole = false;
	mIsCtrlPressed = false;
	mIsShiftPressed = false;
	mEventHandled = 0;


    mOperations.case_<StringDataMsg>(
            [&](MsgSwitch::caseOperationArg const&  val,Systems::ChannelHandler out)
				{
					auto msg = CAST_MSG_TO_TYPE(StringDataMsg,val);
					mEditBox->setText(msg->getData());
					return MsgSwitch::caseOperationRet();
				});

}

void GameConsoleWindow::CreateCEGUIWindow()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	sNamePrefix = ++iInstanceNumber + "_";
	std::string inLayoutName = "console.layout";
	m_ConsoleWindow = pWindowManager->loadWindowLayout(inLayoutName,sNamePrefix);

	if (!m_ConsoleWindow)
	{
		CEGUI::Logger::getSingleton().logEvent("Error: Unable to load the ConsoleWindow from .layout");
	}
	else
	{
		CEGUI::System::getSingleton().setGUISheet(CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", "root"));
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_ConsoleWindow);

		mEditBox = static_cast<CEGUI::Editbox*>(m_ConsoleWindow->getChild(sNamePrefix + "EditBox"));
		mListBox = static_cast<CEGUI::Listbox*>(m_ConsoleWindow->getChild(sNamePrefix + "ChatBox"));

		RegisterHandlers();

		mListBox->getVertScrollbar()->setEndLockEnabled(true);

		mDesc.add_options()
			("help","Usage message")
			("last",value<int>()->zero_tokens(),"Last command")
			("macroRecStart","Start recording macro")
			("macroRecEnd",value<std::string>(),"End recording and saving macro")
			("macroExecute",value<std::vector<std::string>>()->multitoken(),"Execute a pre saved macro");

		//pythonBindingsWorker = boost::thread(&PythonBindings::run,&mPythonBinding);

	}

//	auto thisPtr = this;
//	mPythonBinding.registerLogWriter([=](std::string inVal)
//				{
//					thisPtr->OutputText(inVal);
//				});
}

void GameConsoleWindow::RegisterHandlers()
{
	m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->subscribeEvent(
			CEGUI::Editbox::EventTextAccepted,
			CEGUI::Event::Subscriber(
					&GameConsoleWindow::Handle_TextSubmitted,this));

	m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->subscribeEvent(
						CEGUI::Window::EventKeyUp,
						CEGUI::Event::Subscriber(
								&GameConsoleWindow::Handle_KeyUp,this));

	m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->subscribeEvent(
				CEGUI::Window::EventKeyDown,
				CEGUI::Event::Subscriber(
						&GameConsoleWindow::Handle_KeyDown,this));

}

bool GameConsoleWindow::Handle_KeyUp(const CEGUI::EventArgs &e)
{


	auto keys = static_cast<const CEGUI::KeyEventArgs &>(e);
	if(keys.scancode == CEGUI::Key::LeftControl)
		mIsCtrlPressed = false;
	if(keys.scancode == CEGUI::Key::Space)
		mIsShiftPressed = false;


	return true;
}

bool GameConsoleWindow::Handle_KeyDown(const CEGUI::EventArgs &e)
{
		auto keys = static_cast<const CEGUI::KeyEventArgs &>(e);

		if(keys.scancode == CEGUI::Key::LeftControl)
			mIsCtrlPressed = true;
		if(keys.scancode == CEGUI::Key::Space)
				mIsShiftPressed = true;


		if(mIsShiftPressed && mIsCtrlPressed)
		{
			std::string Msg = mEditBox->getText().c_str();
			std::string LastWord = Msg.substr(Msg.find_last_of(" ")+1);
			{
				//mPythonBinding.pushOperation(PythonBindingsOperations::ExecuteLine,"wordToComplete=\""+LastWord+"\"");
				ListMsg	test = {MAKE_SHARED_MSG_TYPE<ScriptExecuteLine>("wordToComplete=\""+LastWord+"\""),Systems::createMacroMsg("autoComplete.py")};

				auto channel2 = Systems::ChannelHandler::createChannelHandler<Systems::ThreadSafeChannel>("GameConsoleWindow::ExecuteMacro");

				Systems::getSystemBase<Systems::ScriptSystem>()->getChannelIn().addMsg(
						Systems::ChanneledMessages::create(channel2,
								MAKE_SHARED_MSG_TYPE<ListMsg>(test))
				);

				Systems::waitOnChannel(channel2);

				MsgSwitch::caseOperationArg tmp;
				if(channel2.getMsg(tmp))
                    mOperations(tmp,Systems::ChannelHandler());
			}
		}

	return false;
}

bool GameConsoleWindow::Handle_TextSubmitted(const CEGUI::EventArgs &e)
{

	CEGUI::String Msg = mEditBox->getText();
	(this)->ParseText(Msg);

	mEditBox->setText("");

	return true;
}
bool GameConsoleWindow::Handle_SendButtonPressed(const CEGUI::EventArgs &e)
{
	CEGUI::String Msg = mEditBox->getText();
	ParseText(Msg);
	mEditBox->setText("");

	return true;
}
void GameConsoleWindow::ParseText(CEGUI::String inMsg)
{
	std::string inString = inMsg.c_str();
	boost::algorithm::trim(inString);
	mLastLines.push_back(inMsg.c_str());

	if(mIsRecordind)
		mRecordedLines.push_back(inMsg.c_str());

	OutputText(">>"+inMsg);
	if(boost::algorithm::starts_with(inMsg.c_str(),"--"))
	{

		inString = std::string("console ") + inString;

		std::vector<std::string>	res;
		boost::algorithm::split(res,inString,boost::algorithm::is_space());

		//std::cout << "splitted " << res.size() << std::endl;
		boost::program_options::basic_command_line_parser<char> consoleParser(res);
		consoleParser.options(mDesc);

		boost::program_options::variables_map	vm;
		boost::program_options::store(consoleParser.run(),vm);

		if (vm.count("help"))
		{
			std::ostringstream stream;
			mDesc.print(stream);

			OutputText(stream.str());
		}
		if (vm.count("last"))
		{
			if(vm["last"].as<int>() < mLastLines.size())
				mEditBox->setText(mLastLines[vm["last"].as<int>()]);
		}
		if(vm.count("macroRecStart"))
		{
			mIsRecordind = true;
		}
		if(vm.count("macroRecEnd"))
		{
			namespace fs = boost::filesystem;
			mIsRecordind = false;
			mRecordedLines.pop_back();

			auto strFileName = vm["macroRecEnd"].as<std::string>();


			fs::path full_path( fs::initial_path<fs::path>() );

			full_path/= "macros";
			if(!fs::is_directory(full_path))
				fs::create_directory(full_path);

			full_path /= strFileName;

			if(fs::exists(full_path))
				OutputText("overwriting",CEGUI::colour(1,0,0,1));

			std::ofstream	outFile(full_path.c_str());
			std::for_each(std::begin(mRecordedLines),std::end(mRecordedLines),[&](std::string const &  val)
			{
				outFile << val << std::endl;
			});
		}
		if(vm.count("macroExecute"))
		{

			if(vm["macroExecute"].empty())
				OutputText("Failed macroExecute missing macro names list");
			else
			{
				auto vecFileName = vm["macroExecute"].as<std::vector< std::string >>();



				for(auto strFileName = begin(vecFileName);strFileName != end(vecFileName);++strFileName)
				{
					ExecuteMacro(*strFileName);
				}
			}

		}

	}
	else
	{


		auto ret = "";
		Systems::getSystemBase<Systems::ScriptSystem>()->getChannelIn().addMsg(MAKE_SHARED_MSG_TYPE<ScriptExecuteLine>(inMsg.c_str()));
		//mPythonBinding.pushOperation(PythonBindingsOperations::ExecuteLine, inMsg.c_str());
//		if (ret.compare("None") && ret.compare(""))
//			OutputText(ret,CEGUI::colour(1,0,0,1));

	}

}

void GameConsoleWindow::OutputText(CEGUI::String const& inMsg, CEGUI::colour colour)
{
	//CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(m_ConsoleWindow->getChild("ChatBox"));
	CEGUI::FormattedListboxTextItem *newItem=0; // This will hold the actual text and will be the listbox segment / item

	newItem = new CEGUI::FormattedListboxTextItem(inMsg,CEGUI::HTF_WORDWRAP_LEFT_ALIGNED); // Instance the Item with Left

	newItem->setTextColours(colour);
	mListBox->addItem(newItem);
}

void GameConsoleWindow::setVisible(bool visible)
{
	m_ConsoleWindow->setVisible(visible);
	m_bConsole = visible;
	//auto	editBox =  static_cast<CEGUI::Editbox*>(m_ConsoleWindow->getChild(sNamePrefix + "EditBox"));
	if(visible)
		mEditBox->activate();
	else
		mEditBox->deactivate();
}

bool GameConsoleWindow::isVisible()
{
	return m_ConsoleWindow->isVisible();
}



void GameConsoleWindow::ExecuteMacro(std::string strFileName) {

	using namespace Systems;

	auto macroMsg = Systems::createMacroMsg(strFileName);

	if (macroMsg)
	{
		auto channel2 = ChannelHandler::createChannelHandler<ThreadSafeChannel>("GameConsoleWindow::ExecuteMacro");
		Systems::getSystemBase<Systems::ScriptSystem>()->getChannelIn().addMsg(
				ChanneledMessages::create(channel2,macroMsg)
		);
		Systems::waitOnChannel(channel2);

			MsgSwitch::caseOperationArg tmp;
			if(channel2.getMsg(tmp))
                mOperations(tmp,Systems::ChannelHandler());

	}
	else
	{
		OutputText(strFileName + " Doesnt exists",CEGUI::colour(1,0,0,1));
	}
}


//void GameConsoleWindow::OutputText(std::string const& inMsg) {
//	OutputText(inMsg,CEGUI::colour(1,0,0,1));
//}




