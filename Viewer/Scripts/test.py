from Testing import *;

#vec = [];


#a = 1
#print a
#del a

#for iDx in range(0,4):
#	vec.append(msgFactory(iDx))

#for item in vec:
	#openLet = openEnvelope(item)
#	print type(item)
	
#	if item.getMessageType() == Testing.FocusEntity.getMessageType() :
#		print item.getData();

def sendNotev():
	channel2 = Testing.createChannelHandler("initChannel");
	chanMsg = ChanneledMessages.createChanneledMsg(channel2,Testing.NotevMsg(10))
	Testing.getSystemChannelIn(1).addMsg(chanMsg);
	Testing.waitOnChannel(channel2,100,2)
	inMsg = Testing.retMsg(channel2)
	print "The Msg " , type(inMsg) , 
#	if inMsg.getMessageType() == Testing.IntDataMsg.getMessageType() or inMsg.getMessageType() == Testing.FloatDataMsg.getMessageType() or inMsg.getMessageType() == Testing.StringDataMsg.getMessageType():
	print inMsg.getData() ,
	print " the channel " , channel2.numOfInstances
for iDx in range(1,10):
	sendNotev();

testDir = dir(Testing)
print(filter(lambda item: item.find("Msg") != -1, testDir))

