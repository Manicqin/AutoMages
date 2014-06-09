/*
 * PythonBindings.cpp
 *
 *  Created on: Jul 28, 2012
 *      Author: manicqin
 */

#include "PythonBindings.h"

#include "../ClassExports/MessagesExports.h"
#include "../ClassExports/SystemsExports.h"
#include "../ClassExports/OISExports.h"
#include "../ClassExports/ComponentsExports.h"

#include <python2.7/Python.h>
#include <python2.7/import.h>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/filesystem.hpp>

using namespace boost::python;

PythonBindings::PythonBindings(std::string const& scriptDir) {
	std::string tmpScriptDir = scriptDir;
	if (PyImport_AppendInittab("MessagesExports", initMessagesExports) == -1)
					    throw std::runtime_error("Failed to add MessagesExports to the interpreter's "
					                 "built in modules");

	if (PyImport_AppendInittab("SystemsExports", initSystemsExports) == -1)
				    throw std::runtime_error("Failed to add SystemsExports to the interpreter's "
				                 "built in modules");

	if (PyImport_AppendInittab("OISExports", initOISExports) == -1)
				    throw std::runtime_error("Failed to add OISExports to the interpreter's "
				                 "built in modules");

	if (PyImport_AppendInittab("ComponentsExports", initComponentsExports) == -1)
					    throw std::runtime_error("Failed to add ComponentsExports to the interpreter's "
					                 "built in modules");
		Py_Initialize();

//		PyEval_InitThreads();
		main_module = import("__main__");

		main_namespace = main_module.attr("__dict__");
		main_namespace["__builtin__"] = import("__builtin__");
		main_namespace["__main__"] = import("__main__");
		main_namespace["SystemsExports"] = import("SystemsExports");
		main_namespace["MessagesExports"] = import("MessagesExports");
		main_namespace["OISExports"] = import("OISExports");

		if(tmpScriptDir.empty())
		{
			boost::filesystem::path workDir = boost::filesystem::absolute("");
			workDir /= "Scripts/macros/";
			tmpScriptDir = workDir.string();
		}

		PyObject* sysPath = PySys_GetObject("path");
		PyList_Insert(sysPath, 0, PyString_FromString(tmpScriptDir.c_str()));
		//executeLine();
//		auto line = std::string("");
//		auto evalRetVal = exec(line.c_str(),main_namespace);
		//auto outMsg = extract<std::string>(str(evalRetVal));
}

PythonBindings::~PythonBindings() {

}

std::string PythonBindings::executeLine(std::string const& line,Systems::ChannelHandler outChannel) {

	std::string outMsg = "";

	if(outChannel.isChannelOpen())
		main_namespace["__builtin__"].attr("sysChan") = object(ptr(&outChannel));
	try{
		auto evalRetVal = exec(line.c_str(),main_namespace);
		outMsg = extract<std::string>(str(evalRetVal));

	}
	catch (...) {
				//if (PyErr_Occurred())

			PyObject *exc,*val,*tb;
			PyErr_Fetch(&exc,&val,&tb);
			PyErr_NormalizeException(&exc,&val,&tb);
			handle<> hexc(exc);
			handle<> hval(allow_null(val));
			handle<> htb(allow_null(tb));

			object traceback(import("traceback"));
			object format_exception(traceback.attr("format_exception"));
			object formatted_list(format_exception(hexc,hval,htb));
			object formatted(str("").join(formatted_list));
			outMsg = extract<std::string>(formatted);

			}


	return outMsg;
}

std::string PythonBindings::executeFile(const std::string& fileName,Systems::ChannelHandler outChannel) {
	std::string outMsg = "";

	if(outChannel.isChannelOpen())
		main_namespace["__builtin__"].attr("sysChan") = object(ptr(&outChannel));

	try{
		auto evalRetVal = exec_file(fileName.c_str(),main_namespace);
		outMsg = extract<std::string>(str(evalRetVal));
	}
	catch (...) {
			//if (PyErr_Occurred())

		PyObject *exc,*val,*tb;
					PyErr_Fetch(&exc,&val,&tb);
					PyErr_NormalizeException(&exc,&val,&tb);
					handle<> hexc(exc);
					handle<> hval(allow_null(val));
					handle<> htb(allow_null(tb));

					object traceback(import("traceback"));
					object format_exception(traceback.attr("format_exception"));
					object formatted_list(format_exception(hexc,hval,htb));
					object formatted(str("").join(formatted_list));
					outMsg = extract<std::string>(formatted);

		}

	return outMsg;
}

boost::python::object PythonBindings::createScriptObject(
		const std::string& fileName) {
	boost::python::object	retVal;

	try{
		retVal = boost::python::exec_file(fileName.c_str(),main_namespace);

	}
	catch (...) {
			//if (PyErr_Occurred())

		PyObject *exc,*val,*tb;
					PyErr_Fetch(&exc,&val,&tb);
					PyErr_NormalizeException(&exc,&val,&tb);
					handle<> hexc(exc);
					handle<> hval(allow_null(val));
					handle<> htb(allow_null(tb));

					object traceback(import("traceback"));
					object format_exception(traceback.attr("format_exception"));
					object formatted_list(format_exception(hexc,hval,htb));
					object formatted(str("").join(formatted_list));
					std::string outMsg = extract<std::string>(formatted);
					//std::cout << outMsg << std::endl;
		}
	return retVal;
}
