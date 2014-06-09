/*
 * Channel.h
 *
 *  Created on: Sep 23, 2012
 *      Author: manicqin
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <vector>
#include <memory>
#include <iostream>
#include <type_traits>

#include <tbb/concurrent_queue.h>

#include <boost/utility.hpp>
#include "../EntityComponents/Messages.h"

namespace Systems {

class Channel {
public:
	Channel():mChannelName("NoName"){}
	Channel(std::string	const&	channelName):mChannelName(channelName){	}
	virtual ~Channel();

    void addMsg(boost::shared_ptr<Messages> const &  newMsg);
    bool getMsg(boost::shared_ptr<Messages>& outMsg);
    virtual void addMsgImp(boost::shared_ptr<Messages> const &  newMsg) = 0;
    virtual bool getMsgImp(boost::shared_ptr<Messages>& outMsg) = 0;
	virtual const std::size_t hasMsgs() const = 0;
    virtual bool getAllMsgs(std::vector<boost::shared_ptr<Messages>>& vecMessages, std::size_t msgAmount = -1);
    virtual void addAllMsgs(std::vector<boost::shared_ptr<Messages>> const & vecMessages);




	const std::string getChannelName() const {
		return mChannelName;
	}

	void setChannelName(std::string channelName) {
		mChannelName = channelName;
	}

private:

	std::string	mChannelName;

};

class VoidChannel : public Channel{
public:
	VoidChannel(std::string	const&	channelName):Channel(channelName){}
    void addMsgImp(boost::shared_ptr<Messages> const &  newMsg){}
    bool getMsgImp(boost::shared_ptr<Messages>& outMsg) {return false;}
	const std::size_t hasMsgs() const { return 0; }

};
EmptyMessageDeclare(EndMessage);

/*
 *
 *	ChannelHandler uses the shared_ptr use_count in it's internal logics,
 *	When iterating over a channelHandler if the channel has only one instance (unique)
 *	It means that there are no systems writing to the channel so the channelHandler
 *	doesnt need to wait (and can end the iteration).
 *
 *	The channelHandler keeps his instances on a tight leash, that is the reason why every function or constructor
 *	that can add an instance to the shared_ptr is private!
 *
 */

class ChannelHandler : public Channel
{
public:
	typedef	std::shared_ptr<Channel>	innerChannelType;
	ChannelHandler()
	{
		mChannel.reset();
		setChannelName("NoName");
		//std::cout << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
	}
	ChannelHandler(std::string	const&	channelName):Channel(channelName)
	{
		mChannel.reset();
		setChannelName(channelName);
		//std::cout << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
	}

	virtual ~ChannelHandler()
	{
		//std::cout << "~" << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
	}
	ChannelHandler(const ChannelHandler & cpy)
	{
		mChannel = cpy.mChannel;
		setChannelName(cpy.getChannelName());
		//std::cout << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
	}

//	ChannelHandler(const ChannelHandler && cpy):mChannel(std::move(cpy.mChannel))
//	{
//		setChannelName(mChannel->getChannelName());
//	}

	ChannelHandler(innerChannelType channel):Channel(channel->getChannelName()),mChannel(std::move(channel))
	{
		//std::cout << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
	}

	ChannelHandler& operator = (ChannelHandler in)
	{
		mChannel = in.mChannel;
		setChannelName(in.getChannelName());
		//std::cout << getChannelName() << " " << mChannel.get() << " " << mChannel.use_count() << std::endl;
		return *this;
	}

//	ChannelHandler& operator = (ChannelHandler &&in)
//	{
//		if(in.mChannel)
//			std::cout << "ChannelHandler& operator = (ChannelHandler &&in) " << in.numOfInstances() << " " << in.getChannelName() << std::endl;
//		else
//			std::cout << "ChannelHandler& operator = (ChannelHandler &&in) no instance" << std::endl;
//
//	    std::swap(this->mChannel, in.mChannel);
//	    return *this;
//	}

/*
 * T& operator=(T&& x)
{
    swap(*this, x);
    return *this;
}
 */
	template <class T, typename... _Args>
	static ChannelHandler createChannelHandler(_Args&&... __args)
	{
		return ChannelHandler(std::make_shared<T>( std::forward<_Args>(__args)...));
	}

//	template <class T>
//	static innerChannelType createNewChannel(std::string const& channelName = "")
//	{
//		static_assert(std::is_base_of<Channel, T>::value,"ChannelHandler::createNewChannel Can only return a deriver of Channel");
//		return std::make_shared<T>(channelName);
//	}

	const bool isChannelOpen() const
	{
		return mChannel != nullptr;
	}

	const long numOfInstances() const
	{
		return mChannel.use_count();
	}

	void releaseChannel()
	{
		mChannel.reset();
	}

	const std::size_t hasMsgs() const
	{
		return mChannel->hasMsgs();
	}

    virtual void addMsgImp(boost::shared_ptr<Messages> const &  newMsg)
	{
		BOOST_ASSERT_MSG(mChannel,"Cannot AddMsg on closed channel ");
		mChannel->addMsg(newMsg);
	}

    virtual bool getMsgImp(boost::shared_ptr<Messages>& outMsg)
	{
		BOOST_ASSERT_MSG(mChannel,"Cannot getMsg on closed channel ");
		return mChannel->getMsg(outMsg);
	}

	innerChannelType	getChannel()
	{
		BOOST_ASSERT_MSG(mChannel,"Cannot getChannel on closed channel ");
		return mChannel;
	}

	const innerChannelType	getConstChannel()
	{
		BOOST_ASSERT_MSG(mChannel,"Cannot getChannel on closed channel ");
		return mChannel;
	}
private:

//	ChannelHandler(innerChannelType channel)
//	{
//		if(channel)
//			std::cout << "Private ChannelHandler(innerChannelType const &  channel) " << channel.use_count() << " " << channel->getChannelName()  <<std::endl;
//		else
//			std::cout << "Private ChannelHandler(innerChannelType const &  channel) no instance" << std::endl;
//
//		mChannel = channel;
//	}
	static ChannelHandler crateChannelHandler(innerChannelType const channel)
	{
		return ChannelHandler(channel);
	}
	innerChannelType	mChannel;
};

struct ChanneledMessages : public EnumMessages<ChanneledMessages>
{
public:
    ChanneledMessages(ChannelHandler mChannel,boost::shared_ptr<Messages> wrap)
		:EnumMessages<ChanneledMessages>("ChanneledMessages"),mWrapped(wrap),mBackChannel(mChannel)
	{
		//std::cout << "ChanneledMessages " << mBackChannel.getChannelName() << std::endl;
	}

//	ChanneledMessages(ChanneledMessages const& cpy):mWrapped(cpy.mWrapped),mBackChannel(cpy.mBackChannel)
//	{
//		int i = 0;
//		i++;
//	}
	ChannelHandler getBackChannel() const {

		BOOST_ASSERT_MSG(mBackChannel.isChannelOpen(),"ChanneledMessages BackChannel not Open");
		return mBackChannel;
	}

	virtual ~ChanneledMessages()
	{
		//std::cout << "~ChanneledMessages " << mBackChannel.getChannelName() << std::endl;
	}

//	static MsgSwitch::caseOperationArg create(ChannelHandler channelIn,Messages* wrap)
//	{
//		return MsgSwitch::caseOperationArg(new ChanneledMessages(channelIn,MsgSwitch::caseOperationArg(wrap)));
//	}

    static boost::shared_ptr<Messages> create(ChannelHandler  channelIn,boost::shared_ptr<Messages> wrap)
	{
		return MAKE_SHARED_MSG_TYPE<ChanneledMessages>(channelIn,wrap);
	}

    boost::shared_ptr<Messages> getWrapped() const
	{
		return mWrapped;
	}
private:
    boost::shared_ptr<Messages> mWrapped;
	ChannelHandler	const mBackChannel;


//	void setBackChannel(ChannelHandler backChannel) {
//		mBackChannel = backChannel;
//	}
};

class ThreadSafeChannel : public Channel
{
public:
	ThreadSafeChannel(std::string	const&	channelName):Channel(channelName){}
	virtual ~ThreadSafeChannel(){}
    virtual void addMsgImp(boost::shared_ptr<Messages> const & newMsg)
	{
		mQueue.push(newMsg);
	}

    virtual bool getMsgImp(boost::shared_ptr<Messages> &outMsg)
	{
		return mQueue.try_pop(outMsg);
	}

	const std::size_t hasMsgs() const
	{
		return !mQueue.empty();
	}
private:
    tbb::concurrent_queue<boost::shared_ptr<Messages>>	mQueue;
};

} /* namespace Systems */
#endif /* CHANNEL_H_ */
