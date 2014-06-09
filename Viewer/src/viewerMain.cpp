#include <iostream>
#include <vector>
#include <memory>

#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <boost/python/call_method.hpp>

#include "config.h"
#include <EntityComponents/Entity.h>
#include <Systems/SystemBase.h>
#include <Systems/Channel.h>
#include <EntityComponents/AllMessages.h>

using namespace std;
using namespace boost::python;

class A
{
public:
    virtual void speak(){};
};

struct AWrap : A, wrapper<A>
{
    AWrap()
    {
    }

      AWrap(PyObject*)
      {
      }
    void speak()
    {
        this->get_override("speak")();
    }
};

class B1 : public A
{
public:
    virtual void speak()
    {
        std::cout << "B1" << std::endl;
    }
};

class B2 : public A
{
public:
    virtual void speak()
    {
        std::cout << "B2" << std::endl;
    }
};

class C1 : public B1
{
public:
    virtual void speak()
    {
        std::cout << "C1" << std::endl;
    }
};


class C2 : public B2
{
public:
    virtual void speak()
    {
        std::cout << "C2" << std::endl;
    }
};

A* openEnvelope(boost::shared_ptr<A> in)
{
  return in.get();
}

boost::shared_ptr<A> factory2(std::size_t id)
{
    boost::shared_ptr<A> retVal = nullptr;
    switch (id) {
    case 0:
        retVal = boost::make_shared<B1>();
        break;
    case 1:
        retVal = boost::make_shared<B2>();
        break;
    case 2:
        retVal = boost::make_shared<C1>();
        break;
    case 3:
        retVal = boost::make_shared<C2>();
        break;
    default:
        break;
    }

    return retVal;
}

A* factory(std::size_t id)
{
    A* retVal = nullptr;
    switch (id) {
    case 0:
        retVal = new B1;
        break;
    case 1:
        retVal = new B2;
        break;
    case 2:
        retVal = new C1;
        break;
    case 3:
        retVal = new C2;
        break;
    default:
        break;
    }

    return retVal;
}

struct Tie_Them_together
    : with_custodian_and_ward_postcall<1, 0>
{
   typedef reference_existing_object result_converter;
};
/*
 *    Expose_DataMsg_Class(StringDataMsg);
    Expose_DataMsg_Class(IntDataMsg);
    Expose_DataMsg_Class(FloatDataMsg);
    Expose_DataMsg_Class(NotevMsg);
 *
 *
*/
boost::shared_ptr<Messages> msgFactory(std::size_t  id)
{
    boost::shared_ptr<Messages> retVal = nullptr;
    switch (id) {
    case 0:
        retVal = boost::make_shared<StringDataMsg>(boost::lexical_cast<std::string>(id));
        break;
    case 1:
        retVal = boost::make_shared<IntDataMsg>(id);
        break;
    case 2:
        retVal = boost::make_shared<FloatDataMsg>(id+0.0);
        break;
    case 3:
        retVal = boost::make_shared<NotevMsg>(id);
        break;
    case 4:
        retVal = boost::make_shared<FocusEntity>(id);
        break;
    default:
        break;
    }

    return retVal;
}

class TestSystem : public Systems::EnumedSystem <TestSystem>
{
public:
    TestSystem():Systems::EnumedSystem <TestSystem>("TestSystem"){}
    DEFINE_CASE_IMP(createComponent){}
    DEFINE_CASE_IMP(startSystem){}
    DEFINE_CASE_IMP(initSystem){}
    DEFINE_CASE_IMP(shutDownSystem){}

};

#define Base_Msg_Expose(x) 	.def("getMessageType",&x::getItemTypeId).staticmethod("getMessageType")	\
                            .def("createSharedPtr",&x::createSharedPtr)

#define Expose_Entity_Property(x)	.add_property("entityId",&x::getEntityId,&x::setEntityId)

#define Expose_Msg_Class_No_Ctor(x)		class_<x,bases<Messages>,boost::shared_ptr<x>>(#x,no_init) \
            Base_Msg_Expose(x)

#define Expose_Msg_Class(x)		class_<x,bases<Messages>,boost::shared_ptr<x>>(#x,init<x::ClassDeriver>()) \
            Base_Msg_Expose(x)

#define Expose_EmptyMsg_Class(x)		class_<x,bases<Messages>,boost::shared_ptr<x>>(#x) \
            Base_Msg_Expose(x)

#define Expose_DataMsg_Class(x)	class_<x,bases<Messages> , boost::shared_ptr<x> >(#x,init<x::DataType>())	\
        .def("getData",&x::getData)	\
        Base_Msg_Expose(x)


Systems::ChannelHandler	createChannelHandler(std::string const& channelName)
{
    //asm volatile ("int3;");
    return Systems::ChannelHandler::createChannelHandler<Systems::ThreadSafeChannel>(channelName);
}
Systems::ChannelHandler	getSystemChannelIn(std::size_t	const& systemId)
{
    auto system = Systems::SystemsManager::getSingleton().getSystem(systemId);
    BOOST_ASSERT_MSG(system,"No System!!");
    return system->getChannelIn();
}

BOOST_PYTHON_FUNCTION_OVERLOADS(waitOnChannel_Overloads,Systems::waitOnChannel,1,4);


boost::shared_ptr<Messages> retMsg(Systems::ChannelHandler inChannel)
{
    boost::shared_ptr<Messages> retVal = nullptr;
    if (inChannel.isChannelOpen() && inChannel.hasMsgs())
        inChannel.getMsg(retVal);

    return retVal;
}
 //def("retMsg",retMsg);
BOOST_PYTHON_MODULE(Testing)
{

    class_<A , boost::shared_ptr<A> , boost::noncopyable >("A")
        .def("speak", &A::speak);

    class_<B1,bases<A>,boost::shared_ptr<B1>>("B1").def("speak",&B1::speak);
    class_<B2,bases<A>,boost::shared_ptr<B2>>("B2").def("speak",&B2::speak);
    class_<C1,bases<A>,boost::shared_ptr<C1>>("C1").def("speak",&C1::speak);
    class_<C2,bases<A>,boost::shared_ptr<C2>>("C2").def("speak",&C2::speak);
    boost::python::def("waitOnChannel",&Systems::waitOnChannel,waitOnChannel_Overloads());
    def("factory",factory,return_value_policy<manage_new_object>());
    def("factory2",factory2);
    def("msgFactory",msgFactory);

    boost::python::class_<Systems::ChannelHandler>("ChannelHandler",boost::python::init<>())
                        .def(boost::python::init<std::string const&>())
                        .def("addMsg",&Systems::ChannelHandler::addMsg)
                        .def("getMsg",&Systems::ChannelHandler::getMsg)
                        .def("releaseChannel",&Systems::ChannelHandler::releaseChannel)
                        .add_property("numOfInstances",&Systems::ChannelHandler::numOfInstances)
                        .add_property("hasMsgs",&Systems::ChannelHandler::hasMsgs)
                        .add_property("channelName",&Systems::ChannelHandler::getChannelName);


    class_<Messages,boost::shared_ptr<Messages>>("Messages")
        .def("getMessageType",&Messages::getMessageType);

    def("retMsg",retMsg);
    Expose_DataMsg_Class(StringDataMsg);
    Expose_DataMsg_Class(IntDataMsg);
    Expose_DataMsg_Class(FloatDataMsg);
    Expose_DataMsg_Class(NotevMsg);
    Expose_DataMsg_Class(FocusEntity);

    typedef	boost::shared_ptr<Messages>(*createChanneledPtr)(Systems::ChannelHandler channelIn,boost::shared_ptr<Messages> wrap);

    class_<Systems::ChanneledMessages,bases<Messages>>("ChanneledMessages",init<Systems::ChannelHandler,boost::shared_ptr<Messages>>())
        Base_Msg_Expose(Systems::ChanneledMessages)
        .def("createChanneledMsg",(createChanneledPtr)&Systems::ChanneledMessages::create).staticmethod("createChanneledMsg");

    boost::python::def("createChannelHandler",createChannelHandler);

    boost::python::def("getSystemChannelIn",&getSystemChannelIn);

  //  def("openEnvelope",openEnvelope,Tie_Them_together());
}


int main() {
    EntityComponent::EntityRegistry cEntRgstr;

    Systems::SystemsManager	mngr;

    mngr.addSystem(new TestSystem)		->setChannelIn(Systems::ChannelHandler::createChannelHandler<Systems::ThreadSafeChannel>("LoggingSystemIn"));
    mngr.start();
    int iDx = 0;
    Systems::getSystemBase<TestSystem>()->getOperations().case_<NotevMsg>([&](MsgSwitch::caseOperationArg const& val,Systems::ChannelHandler out)->MsgSwitch::caseOperationRet
    {
        auto msg = static_cast<NotevMsg *>(val.get());
        std::cout << msg->getData() << std::endl;
        if(out.isChannelOpen())
        out.addMsg(msgFactory(iDx));
        if (iDx < 4)
            iDx ++;
        else
            iDx = 0;
        return msgSwitch::returnOk;

    });

    Systems::getSystemBase<TestSystem>()->getChannelIn().addMsg(boost::make_shared<NotevMsg>());
    std::cout << Systems::getSystemBase<TestSystem>()->getSystemId() <<std::endl;
    if (PyImport_AppendInittab("Testing", initTesting) == -1)
        throw std::runtime_error("Failed to add ComponentsExports to the interpreter's "
                     "built in modules");


    Py_Initialize();


    auto main_module = import("__main__");

    auto main_namespace = main_module.attr("__dict__");
    main_namespace["__builtin__"] = import("__builtin__");
    main_namespace["__main__"] = import("__main__");
    main_namespace["Testing"] = import("Testing");
    //main_namespace["MessagesExports"] = import("MessagesExports");
    //main_namespace["SystemsExports"] = import("SystemsExports");

    boost::filesystem::path pthScrpt = Scripts_Path;
    pthScrpt /= R"fileName(/test.py)fileName";

    std::string outMsg;
    try{

        auto evalRetVal = exec_file(pthScrpt.c_str(),main_namespace);
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


    std::cout <<outMsg << std::endl;
   // boost::this_thread::sleep(boost::posix_time::seconds(600));
//    std::vector<std::shared_ptr<A>> cVec {std::make_shared<B1>() , std::make_shared<B2>() , std::make_shared<C1>() , std::make_shared<C2>()};
//    std::for_each(std::begin(cVec),std::end(cVec),
//                  [](std::shared_ptr<A> item){
//                        item->speak();
//                            });
    return 0;
}
