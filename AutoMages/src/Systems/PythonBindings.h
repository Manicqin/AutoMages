/*
 * PythonBindings.h
 *
 *  Created on: Jul 28, 2012
 *      Author: manicqin
 */

#ifndef PYTHONBINDINGS_H_
#define PYTHONBINDINGS_H_

#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <python2.7/abstract.h>
#include "../Systems/Channel.h"
//
//class writer
//{
//public:
//
//	static void	write(std::string strText)
//	{
//		mWriter(strText);
//	}
//
//	static std::function<void(std::string)> mWriter;
//};

//	template <class T>
//		void executeFunction(std::string const& functionName , PyObject* pythonObject , T* val) {
//		using namespace boost;
//
//		auto tmpObj = pythonObject;
//
//		static boost::mutex	mutex;
//		boost::lock_guard<boost::mutex> lock(mutex);
//		python::call_method<void,T*>(tmpObj,functionName.c_str(),val);
//	}

class PythonBindings{
public:
	PythonBindings(std::string const& scriptDir = "");
	virtual ~PythonBindings();

	std::string	executeLine(std::string const& line,Systems::ChannelHandler outChannel);
	std::string executeFile(std::string const& fileName,Systems::ChannelHandler outChannel);

	std::string	executeLineNoBackChannel(std::string const& line,Systems::ChannelHandler outChannel);
	std::string executeFileNoBackChannel(std::string const& fileName,Systems::ChannelHandler outChannel);

	boost::python::object	createScriptObject(std::string const& fileName);

	boost::python::object main_module;
    boost::python::object main_namespace;

};
///////////////////Temp Code
//// class to be wrapped
//class Base
//{
// public:
//   virtual char const* class_name() const { return "Base"; }
//   virtual ~Base();
//};
//
//bool is_base(Base* b)
//{
//   return !std::strcmp(b->class_name(), "Base");
//}
//
//// Wrapper code begins here
//using namespace boost::python;
//
//// Callback class
//class Base_callback : public Base
//{
// public:
//   Base_callback(PyObject* self) : m_self(self) {}
//
//   char const* class_name() const { return call_method<char const*>(m_self, "class_name"); }
//   char const* Base_name() const { return Base::class_name(); }
// private:
//   PyObject* const m_self;
//};
//
//using namespace boost::python;
//BOOST_PYTHON_MODULE(my_module)
//{
//    def("is_base", is_base);
//
//    class_<Base,Base_callback, boost::noncopyable>("Base")
//        .def("class_name", &Base_callback::Base_name)
//        ;
//
//}

#endif /* PYTHONBINDINGS_H_ */
