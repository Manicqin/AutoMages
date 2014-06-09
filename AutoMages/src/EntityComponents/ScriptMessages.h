/*
 * ScriptMessages.h
 *
 *  Created on: Jul 31, 2013
 *      Author: manicqin
 */

#ifndef SCRIPTMESSAGES_H_
#define SCRIPTMESSAGES_H_

#include "Messages.h"

DataMessageDeclare(ScriptExecuteLine,std::string);
DataMessageDeclare(ScriptExecuteFile,std::string);
//DataMessageDeclare3(ScriptCallFunctionMsg,std::string,std::size_t,Entity,boost::shared_ptr<Messages>,Arg);

struct ScriptCallFunctionMsg : public DataMsg<std::string, ScriptCallFunctionMsg>
		{
			public:
			ScriptCallFunctionMsg(std::string const& data):DataMsg(data,"ScriptCallFunctionMsg") { nullifier(mEntity); nullifier(mArg); }
			virtual ~ScriptCallFunctionMsg() = default;
			protected: std::size_t mEntity;
	public: std::size_t getEntity() const { return mEntity;}
	public: void setEntity(std::size_t const& value){ mEntity = value;}
			protected: boost::shared_ptr<Messages> mArg;
	public: boost::shared_ptr<Messages> getArg() const { return mArg;}
	public: void setArg(boost::shared_ptr<Messages>  value){ mArg = value;}
		};

DataMessageDeclare2(CreateScriptComponentMsg,std::shared_ptr<rapidxml::xml_node<>>,std::size_t,EntityId)



template<class T>
 struct ScriptCallFunction2Msg : public DataMsg<std::string, ScriptCallFunction2Msg<T>>
		{
			public:
			ScriptCallFunction2Msg(std::string const& data):DataMsg<std::string, ScriptCallFunction2Msg<T>>(data,"ScriptCallFunction2Msg") { nullifier(mEntity); nullifier(mArg); }
			virtual ~ScriptCallFunction2Msg() = default;
			protected: std::size_t mEntity;
			public: std::size_t getEntity() const { return mEntity;}
			public: void setEntity(std::size_t const& value){ mEntity = value;}
			protected: boost::shared_ptr<T> mArg;
			public: boost::shared_ptr<T> getArg() const { return mArg;}
			public: void setArg(boost::shared_ptr<T> const& value){ mArg = value;}
		};

#endif /* SCRIPTMESSAGES_H_ */
