import SystemsExports;
import MessagesExports;

def printSystemId(x):
    SystemsExports.LogFunc(x + " " + str(SystemsExports.getSystemByName(x)));

def linkSystems(recvSystemId,sendSystemId):
    SystemsExports.setSystemChannelOut(sendSystemId,SystemsExports.getSystemChannelIn(recvSystemId));

def createChanneledMsg(msg,chann):
    return MessagesExports.ChanneledMessages.createChanneledMsg(chann,msg.createSharedPtr());

class sender():
    mSender = 0;
    def __init__(self,sender):
        mSender = sender;
    def reciever(self,reciever):
        SystemsExports.setSystemChannelOut(mSender,SystemsExports.getSystemChannelIn(reciever));

class linkOutChannel():
    mSender = 0;
    def __init__(self,sender):
        self.mSender = sender;
    def to(self,reciever):
        SystemsExports.setSystemChannelOut(self.mSender,SystemsExports.getSystemChannelIn(reciever));
        
def waitOnChannel(channel):
    flag = True
    while flag:
        time.sleep(1)
        print "waiting on " + channel.channelName + " " + str(channel.numOfInstances)
        if (channel.numOfInstances <= 2):
            flag = False
                


SystemsExports.LogFunc("start")
printSystemId("LoggingSystem");
printSystemId("OgreSystem");
printSystemId("ScriptSystem");
printSystemId("PhysicsSystem");
printSystemId("ComponentsSystem");
printSystemId("ControllerSystem");
printSystemId("GuiSystem");

physSysId   = SystemsExports.getSystemByName("PhysicsSystem");
ogreSysId   = SystemsExports.getSystemByName("OgreSystem");
cntrlSysId  = SystemsExports.getSystemByName("ControllerSystem");
cmpntSysId  = SystemsExports.getSystemByName("ComponentsSystem")
entitySysId = SystemsExports.getSystemByName("EntitySystem")
GuiSysId    = SystemsExports.getSystemByName("GuiSystem")