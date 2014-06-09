/*
 * LogMessages.h
 *
 *  Created on: Oct 25, 2012
 *      Author: manicqin
 */

#ifndef LOGMESSAGES_H_
#define LOGMESSAGES_H_
#include <iostream>
#include <memory>
#include <utility>
#include "Messages.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>

class LogMessage : public EnumMessages<LogMessage>
{
public:
	LogMessage(std::string const& msg,int const& priority = 0 , int const& type = 0 , bool const& console = true, bool const& newLine = true):
					EnumMessages<LogMessage>("LogMessage"),
					mMsg(msg),
					mTime(boost::posix_time::microsec_clock::universal_time()),
					mConsole(console),
					mNewLine(newLine)
	{

	}

	LogMessage(std::string const&& msg,int const& priority = 0 , int const& type = 0 , bool const& console = true, bool const& newLine = true):
	EnumMessages<LogMessage>("LogMessage"),
	mMsg(std::move<std::string const&>(msg)),
						mTime(boost::posix_time::microsec_clock::universal_time()),
						mConsole(console),
						mNewLine(newLine)
	{

	}

	virtual ~LogMessage(){}
	const std::string& getMsg() const {
		return mMsg;
	}

	std::string	getTime() const{

        //return boost::posix_time::to_simple_string(mTime - mGeneralTime);
        return std::to_string(boost::posix_time::time_duration(mTime- mGeneralTime).ticks());
	}

	const bool isConsole() const
	{
		return mConsole;
	}

	const bool isNewLine() const
	{
		return mNewLine;
	}

private:
	const 	std::string 				mMsg;
	static 	boost::posix_time::ptime	mGeneralTime;
	const 	boost::posix_time::ptime	mTime;
	const	bool						mConsole;
	const	bool						mNewLine;

};



#endif /* LOGMESSAGES_H_ */
