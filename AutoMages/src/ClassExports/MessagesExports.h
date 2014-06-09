/*
 * MessageMangers.h
 *
 *  Created on: Oct 1, 2012
 *      Author: manicqin
 */

#ifndef MESSAGEMANGERS_H_
#define MESSAGEMANGERS_H_
#include "../Utilities/rapidxml/rapidxml.hpp"
#include "../EntityComponents/AllMessages.h"
#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <python2.7/import.h>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <memory>

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

//void testInheritRef(boost::shared_ptr<Messages> & val)
//{
//    val = boost::make_shared<IntDataMsg>(10);
//}

BOOST_PYTHON_MODULE(MessagesExports)
{
	using namespace boost::python;

//    def("testInheritRef",testInheritRef);

	class_<std::vector<std::string> >("string_vector")
	        .def(vector_indexing_suite<std::vector<std::string> >());

	typedef	std::string(*getItemName)(std::size_t);
	class_<FamilyRegistry<Messages_Family> >("MessagesRegistry")
		.def("getItemName",(getItemName)&FamilyRegistry<Messages_Family>::getItemName).staticmethod("getItemName");

    class_<Messages,boost::shared_ptr<Messages>>("Messages")
		.def("getMessageType",&Messages::getMessageType);

    //class_<boost::shared_ptr<Messages>>("MessagesSharedPntr");

	Expose_DataMsg_Class(StringDataMsg);
	Expose_DataMsg_Class(IntDataMsg);
	Expose_DataMsg_Class(FloatDataMsg);



//////////////////////////////////////////////////////////COMPONENTMESSAGES_H_
    Expose_Msg_Class_No_Ctor(CreateEntityMsg)
				.def("createFromData",createFromData<CreateEntityMsg>).staticmethod("createFromData")
				.def("createFromFile",createFromFile<CreateEntityMsg>).staticmethod("createFromFile");

    Expose_Msg_Class_No_Ctor(CreateSceneMsg)
                .def("createFromData",createFromData<CreateSceneMsg>).staticmethod("createFromData")
				.def("createFromFile",createFromFile<CreateSceneMsg>).staticmethod("createFromFile");

//////////////////////////////////////////////////////////SYSTEMSMESSAGES_H_

    Expose_Msg_Class_No_Ctor(CreateComponentMsg)
				.def("createFromData",createFromData<CreateComponentMsg>).staticmethod("createFromData")
				.def("createFromFile",createFromFile<CreateComponentMsg>).staticmethod("createFromFile")
				Expose_Entity_Property(CreateComponentMsg);

    Expose_Msg_Class_No_Ctor(InitSystemMsg)
				.def("createFromData",createFromData<InitSystemMsg>).staticmethod("createFromData")
				.def("createFromFile",createFromFile<InitSystemMsg>).staticmethod("createFromFile");
//////////////////////////////////////////////////////////ENTITYMESSAGES_H_

	Expose_DataMsg_Class(FocusEntity);

//////////////////////////////////////////////////////////CONTROLLERMESSAGES_H_

	Expose_EmptyMsg_Class(CaptureMsg);

	Expose_DataMsg_Class(ControllerMouseMoveMsg);

	Expose_DataMsg_Class(ControllerKeyMsg)
		.add_property("down",&ControllerKeyMsg::getDown);

	Expose_DataMsg_Class(ControllerMousePressedMsg)
		.add_property("buttonID",&ControllerMousePressedMsg::getBid)
		.add_property("down",&ControllerMousePressedMsg::getDown);

	Expose_DataMsg_Class(ControllerBufferedMouseMoveMsg);

	Expose_DataMsg_Class(ControllerBufferedKeyMsg)
		.add_property("down",&ControllerBufferedKeyMsg::getDown,&ControllerBufferedKeyMsg::setDown)
		.add_property("interval",&ControllerBufferedKeyMsg::getInterval,&ControllerBufferedKeyMsg::setInterval)
		.add_property("overall",&ControllerBufferedKeyMsg::getOverall,&ControllerBufferedKeyMsg::setOverall);

	Expose_DataMsg_Class(ControllerBufferedMousePressedMsg)
		.add_property("down",&ControllerBufferedMousePressedMsg::getDown,&ControllerBufferedMousePressedMsg::setDown)
		.add_property("interval",&ControllerBufferedMousePressedMsg::getInterval,&ControllerBufferedMousePressedMsg::setInterval)
		.add_property("overall",&ControllerBufferedMousePressedMsg::getOverall,&ControllerBufferedMousePressedMsg::setOverall);

	Expose_DataMsg_Class(ControllerSubscribedKeyMsg)
		.add_property("down",&ControllerSubscribedKeyMsg::getDown,&ControllerSubscribedKeyMsg::setDown)
		.add_property("interval",&ControllerSubscribedKeyMsg::getInterval,&ControllerSubscribedKeyMsg::setInterval)
		.add_property("overall",&ControllerSubscribedKeyMsg::getOverall,&ControllerSubscribedKeyMsg::setOverall);
	/*
	 * bool isDown = false;
	float interval = 0.0f;
	keySet	keys;
	 */

	boost::python::class_<keySet >("KeyArray")
				.def(boost::python::vector_indexing_suite<keySet>());

	boost::python::class_<mouseSet >("MouseKeyArray")
				.def(boost::python::vector_indexing_suite<mouseSet>());

	class_<KeySubscribtion>("KeySubscribtion")
			.def_readwrite("down",&KeySubscribtion::isDown)
			.def_readwrite("up",&KeySubscribtion::isUp)
			.def_readwrite("downInterval",&KeySubscribtion::downInterval)
			.def_readwrite("keys",&KeySubscribtion::keys)
			.def_readwrite("mouseKeys",&KeySubscribtion::mouseKeys)
			.def_readwrite("subscriber",&KeySubscribtion::subscriber);

	Expose_DataMsg_Class(SubscribeToInputMsg)
		.add_property("SubscribtionData",&SubscribeToInputMsg::getSubscribtionData,&SubscribeToInputMsg::setSubscribtionData);


//////////////////////////////////////////////////////////GRAPHICSMESSAGES_H_

	Expose_DataMsg_Class(CastRayMsg)
		.def(init<Ogre::Real const& , Ogre::Real const&>());

	Expose_DataMsg_Class(MoveCameraMsg)
		.def(init<Ogre::Real const& , Ogre::Real const& , Ogre::Real const&>())
		.add_property("cameraId",&MoveCameraMsg::getCameraID,&MoveCameraMsg::setCameraID);

    Expose_DataMsg_Class(VectorMsg);

//////////////////////////////////////////////////////////PHYSICSMESSAGES_H_

	Expose_DataMsg_Class(ApplyForceMsg)
		.def("create",createDataMsg<ApplyForceMsg,btVector3,const btScalar&,const btScalar&,const btScalar&>).staticmethod("create")
		Expose_Entity_Property(ApplyForceMsg);

	Expose_DataMsg_Class(ApplyImpulseMsg)
		.def("create",createDataMsg<ApplyImpulseMsg,btVector3,const btScalar&,const btScalar&,const btScalar&>).staticmethod("create")
		Expose_Entity_Property(ApplyImpulseMsg);

	Expose_DataMsg_Class(ApplyTorqueMsg)
		.def("create",createDataMsg<ApplyTorqueMsg,btVector3,const btScalar&,const btScalar&,const btScalar&>).staticmethod("create")
		Expose_Entity_Property(ApplyTorqueMsg);

	Expose_DataMsg_Class(ApplyTorqueImpulseMsg)
		.def("create",createDataMsg<ApplyTorqueImpulseMsg,btVector3,const btScalar&,const btScalar&,const btScalar&>).staticmethod("create")
		Expose_Entity_Property(ApplyTorqueImpulseMsg);

	Expose_DataMsg_Class(ApplyLinearVelocityMsg)
		.def("create",createDataMsg<ApplyLinearVelocityMsg,btVector3,const btScalar&,const btScalar&,const btScalar&>).staticmethod("create")
		Expose_Entity_Property(ApplyLinearVelocityMsg);


	Expose_DataMsg_Class(ClearForces);

//////////////////////////////////////////////////////////SYSTEMSMESSAGES_H_

	Expose_EmptyMsg_Class(SystemKillMsg);
	Expose_EmptyMsg_Class(StartSystemMsg);
	Expose_EmptyMsg_Class(ShutDownSystemMsg);
	Expose_EmptyMsg_Class(SystemTouchMsg);
	Expose_DataMsg_Class(NotevMsg);
		
    typedef	boost::shared_ptr<Messages>(*createChanneledPtr)(Systems::ChannelHandler channelIn,boost::shared_ptr<Messages> wrap);

    class_<Systems::ChanneledMessages,bases<Messages>>("ChanneledMessages",init<Systems::ChannelHandler,boost::shared_ptr<Messages>>())
		Base_Msg_Expose(Systems::ChanneledMessages)
		.def("createChanneledMsg",(createChanneledPtr)&Systems::ChanneledMessages::create).staticmethod("createChanneledMsg");

}


#endif /* MESSAGEMANGERS_H_ */
