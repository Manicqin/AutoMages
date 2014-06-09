/*
 * Messages.h
 *
 *  Created on: Mar 1, 2012
 *      Author: manicqin
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <type_traits>
#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <functional>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "../Utilities/SelfEnumClasses.h"
#include "../Utilities/MacroUtils.h"

class Messages;
struct Messages_Family{};
class MessagesRegistry : public FamilyRegistry<Messages_Family>
{
public:
	static const std::set<std::size_t> getSubscribers(std::size_t const& msgId){
		return mSubscribers[msgId];
	}

	static void addSubscriber(std::size_t const& msgId,std::size_t const& subscriber) {
		mSubscribers[msgId].insert(subscriber);
	}

	static 	std::map<std::size_t,std::set<std::size_t>>			mSubscribers;

};


struct Messages
{
public:
	Messages():mMessageType(0){}
	virtual ~Messages() {}

	const std::size_t getMessageType() const	{ return mMessageType; }
//	static	boost::shared_ptr<Messages> createMsg(Messages* val)
//	{
//		return boost::shared_ptr<Messages>(val);
//	}

protected:
    std::size_t	mMessageType;

};


struct hashMsg{
    std::size_t operator()(Messages const& msg)
    {
        return msg.getMessageType();
    }

    std::size_t operator()(boost::shared_ptr<Messages> const& msg)
    {
        return msg->getMessageType();
    }


};

template<typename _Key, typename _Func, class _ArgToKey>
class MsgSwitchT
{
private:
    std::map<_Key,_Func>    mFuncMap;
    _Func                   mDefault;
public:
    typedef typename _Func::first_argument_type caseOperationArg;
    typedef _Func                               caseOperation;
    typedef typename _Func::result_type         caseOperationRet;
    typedef _Key                                caseOperationKey;
    template <class T>
    MsgSwitchT& case_(_Func func,T* ignore = nullptr)
    {
        mFuncMap.insert(std::make_pair(_ArgToKey()(*ignore),func));
        return *this;
    }

    MsgSwitchT& case_(_Func func,_Key key)
    {
        mFuncMap.insert(std::make_pair(key,func));
        return *this;
    }

    MsgSwitchT& default_(_Func func)
    {
        mDefault = func;
        return *this;
    }

    template <typename _Arg1 = typename _Func::first_argument_type, typename... Args>
    typename _Func::result_type operator()(typename _Func::first_argument_type const& val, Args... args)
    {
        return execute(val , args...);
    }

    template <typename _Arg1 = typename _Func::first_argument_type, typename... Args>
    typename _Func::result_type execute(typename _Func::first_argument_type const& val, Args... args)
    {
        typename _Func::result_type retVal;
        auto funcIter = mFuncMap.find(_ArgToKey()(val));
        if(funcIter != mFuncMap.end())
            retVal = funcIter->second(val,args...);
        else
            retVal = mDefault(val,args...);
        return retVal;
    }
};

#define	CASE_OPERATION_ARG_DEF_VAL			boost::make_shared<Messages>()
#define	CAST_MSG_TO_TYPE(type,msg)			boost::dynamic_pointer_cast<type>(msg)
#define MAKE_SHARED_MSG_TYPE				boost::make_shared

enum	class msgSwitch : std::size_t{
    returnOk = 0, returnMoveToOutChannel , returnRecycle , continueWork , returnError
};
class EmptyMsg;



template<class D>
struct EnumMessages : public SelfEnumedClass<Messages_Family,D> , public Messages
{
	typedef	SelfEnumedClass<Messages_Family,D> SelfEnumarationClass;
	typedef Messages PolymorphBase;

	EnumMessages(){
		Messages::mMessageType = SelfEnumedClass<Messages_Family,D>::getItemTypeId();
	}
	EnumMessages(std::string const& msgName):SelfEnumedClass<Messages_Family,D>(msgName){
		Messages::mMessageType = SelfEnumedClass<Messages_Family,D>::getItemTypeId();
	}


    boost::shared_ptr<Messages>	createSharedPtr()
    {
        return boost::shared_ptr<Messages>(new D(*dynamic_cast<D*>(this)));
    }
	virtual ~EnumMessages() {}

};

template <class T ,class D>
struct DataMsg : public EnumMessages<D>
{
public:

	typedef T	DataType;

	DataMsg(DataType const & newVal):mData(newVal){}
	DataMsg(DataType const & newVal,std::string const & msgName):EnumMessages<D>(msgName),mData(newVal){}
	virtual ~DataMsg() {}

	const DataType getData() const { return mData; }
private:
	const DataType mData;
};

template <class Ret, class Type , class ...Args>
Ret	createDataMsg(const Args&... args)
{
	return Ret(Type(args...));
}

template <class T ,class D>
struct DataComposerMsg : public EnumMessages<D>
{
public:
	typedef T				DataType;
	typedef std::vector<T>	VectorType;

	DataComposerMsg() = default;
	DataComposerMsg(std::string const & msgName):EnumMessages<D>(msgName){}
	DataComposerMsg(std::string const & msgName,std::initializer_list<T> l):EnumMessages<D>(msgName),mData(l){}
	virtual ~DataComposerMsg() {}

	VectorType getData() { return mData; }
	void addData(T const& mItem)	{	mData.push_back(mItem); }
private:
	VectorType mData;
};

#define EmptyMessageDeclare(msgName)	struct msgName : public EnumMessages<msgName>	{ msgName():EnumMessages<msgName>(#msgName){} virtual ~msgName() = default; };

#define MessageDeclare(msgName,MT1,Mx1)	struct msgName : public EnumMessages<msgName>	\
{ \
	public:	\
	msgName(MT1 const& newX1):EnumMessages<msgName>(#msgName),NameProperty(Mx1)(newX1) { } \
	virtual ~msgName() = default;	\
	DeclareProperty(MT1,Mx1)	\
};


#define MessageDeclare2(msgName,MT1,Mx1,MT2,Mx2)	struct msgName : public EnumMessages<msgName>	\
{ \
	public:	\
	msgName(MT1 const& newX1,MT2 const& newX2):EnumMessages<msgName>(#msgName),NameProperty(Mx1)(newX1),NameProperty(Mx2)(newX2) { } \
	virtual ~msgName() = default;	\
	DeclareProperty(MT1,Mx1)	\
	DeclareProperty(MT2,Mx2)	\
};

#define MessageDeclare3(msgName,MT1,Mx1,MT2,Mx2,MT3,Mx3)	struct msgName : public EnumMessages<msgName>	\
{ \
	public:	\
	msgName(MT1 const& newX1,MT2 const& newX2,MT3 const& newX3):EnumMessages<msgName>(#msgName)	\
							,NameProperty(Mx1)(newX1),NameProperty(Mx2)(newX2),NameProperty(Mx3)(newX3) { } \
	virtual ~msgName() = default;	\
	DeclareProperty(MT1,Mx1)	\
	DeclareProperty(MT2,Mx2)	\
	DeclareProperty(MT3,Mx3)	\
};

#define DataMessageDeclareBegin(msgName , T) struct msgName : public DataMsg<T, msgName>	\
		{ \
			public:	\
			msgName(T const& data):DataMsg(data,#msgName) {} \
			virtual ~msgName() = default;

#define DataMessageDeclareEnd };

#define DataMessageDeclareEnd2(MT1 , Mx1) DeclareProperty(MT1,Mx1)	\
	};

#define DataMessageDeclareEnd3(MT1 , Mx1, MT2 , Mx2) DeclareProperty(MT1,Mx1)	\
		DeclareProperty(MT2,Mx2)	\
	};

#define DataMessageDeclareEnd4(MT1 , Mx1, MT2 , Mx2) DeclareProperty(MT1,Mx1)	\
		DeclareProperty(MT2,Mx2)	\
		DeclareProperty(MT3,Mx3)	\
	};

#define DataMessageDeclare(msgName , T)	DataMessageDeclareBegin(msgName , T)	\
		DataMessageDeclareEnd


#define DataMessageDeclare2(msgName , T , MT1 , Mx1) struct msgName : public DataMsg<T, msgName>	\
		{ \
			public:	\
			msgName(T const& data):DataMsg(data,#msgName) { nullifier(NameProperty(Mx1)); } \
			virtual ~msgName() = default;	\
			DeclareProperty(MT1,Mx1)	\
		};

#define DataMessageDeclare3(msgName , T , MT1 , Mx1 , MT2 , Mx2) struct msgName : public DataMsg<T, msgName>	\
		{ \
			public:	\
			msgName(T const& data):DataMsg(data,#msgName) { nullifier(NameProperty(Mx1)); nullifier(NameProperty(Mx2)); } \
			virtual ~msgName() = default;	\
			DeclareProperty(MT1,Mx1)	\
			DeclareProperty(MT2,Mx2)	\
		};

#define DataMessageDeclare4(msgName , T , MT1 , Mx1 , MT2 , Mx2 , MT3 , Mx3) struct msgName : public DataMsg<T, msgName>	\
		{ \
			public:	\
			msgName(T const& data):DataMsg(data,#msgName) { nullifier(NameProperty(Mx1)); nullifier(NameProperty(Mx2)); nullifier(NameProperty(Mx3)); } \
			virtual ~msgName() = default;	\
			DeclareProperty(MT1,Mx1)	\
			DeclareProperty(MT2,Mx2)	\
			DeclareProperty(MT3,Mx3)	\
		};

#define DataComposerMessageDeclare(msgName , T)	struct msgName : public DataComposerMsg<T, msgName>	\
{ \
	public:	\
	msgName():DataComposerMsg(#msgName) { } \
	msgName(std::initializer_list<T> l):DataComposerMsg(#msgName,l) { } \
	virtual ~msgName() = default;	\
};

DataMessageDeclare(StringDataMsg,std::string);
DataMessageDeclare(IntDataMsg,int);
DataMessageDeclare(FloatDataMsg,float);
DataMessageDeclare(genMsg,std::function<void(void)>);

DataComposerMessageDeclare(ListMsg,boost::shared_ptr<Messages>);
DataComposerMessageDeclare(StringComposerMsg,std::string);
EmptyMessageDeclare(EmptyMsg);
//template <class T>
//boost::optional<MsgSwitch::caseOperationArg>	synthesisMessage(MsgSwitch::caseOperationArg val , std::function<boost::optional<MsgSwitch::caseOperationArg>(std::shared_ptr<T>)> func)
//{
//	return func(std::dynamic_pointer_cast<T>(val));
//}

#endif /* MESSAGES_H_ */
