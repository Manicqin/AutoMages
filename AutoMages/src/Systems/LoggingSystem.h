/*
 * LoggingSystem.h
 *
 *  Created on: Oct 25, 2012
 *      Author: manicqin
 */

#ifndef LOGGINGSYSTEM_H_
#define LOGGINGSYSTEM_H_

#include "SystemBase.h"

#include <string>
#include <fstream>

#include <boost/format/format_fwd.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>
namespace Systems {

class LoggingSystem: public EnumedSystem <LoggingSystem> {
public:
	LoggingSystem();
	virtual ~LoggingSystem();

	//virtual void operator()(std::shared_ptr<Messages>	 const& incomingMsg, ChannelHandler outChannel);
    DEFINE_CASE_PROTO(startSystem);
    DEFINE_CASE_PROTO(createComponent);
    DEFINE_CASE_PROTO(initSystem);
    DEFINE_CASE_PROTO(shutDownSystem);

	std::ofstream	mOutFile;
};

} /* namespace Systems */


BOOST_PARAMETER_NAME(priority)
BOOST_PARAMETER_NAME(type)
BOOST_PARAMETER_NAME(console)
BOOST_PARAMETER_NAME(newLine)

class LogMsg
{
public:

			LogMsg(int const& priority = 0 , int const& type = 0 , bool const& console = true , bool const& newLine = true );
	void	Log(std::string const && msg);
	void	Log_(std::string const & msg);
	void	operator << (boost::format const & os);
	void	operator << (boost::format && os);
	void	operator << (std::string const & msg);
	void	operator << (std::string const && msg);

private:
	int 	const mPriority = 0;
	int 	const mType 	= 0;
	bool 	const mConsole 	= true;
	bool 	const mNewLine 	= true;

};

BOOST_PARAMETER_FUNCTION(
		(LogMsg) , LOG , tag , (optional
				(priority	, 	*,	0)
				(type 		,	*,	0)
				(console	,	*,	true)
				(newLine	,	*,	true)
				)
)
{
	return	LogMsg(priority,type,console,newLine);
}
#endif /* LOGGINGSYSTEM_H_ */
