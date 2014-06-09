#!/usr/bin/python

from SystemWrapperBase import *;
import time
try:
    print ("Importing engine environment")
  
    import SystemsExports;
    import OISExports;
    import MessagesExports;
except ImportError:
    print ("No engine environment")
    
    from MockEngine import *
    
import inspect
from datetime import datetime;
import itertools
from macpath import basename

@logger
def sendControllerSubscribedKeyMsg(subscriber , keyId , down = False,interval = 0.0, overall = 0.0):
    msg = MessagesExports.ControllerSubscribedKeyMsg(keyId)
    msg.interval = interval
    msg.down = down
    msg.overall = overall
    SystemsExports.getSystemChannelIn(subscriber).addMsg(msg.createSharedPtr());

def sendControllerSubscribedKeyMsgStub(subscriber , keyId , down = False,interval = 0.0, overall = 0.0):
    print('         sendControllerSubscribedKeyMsgStub' , subscriber , keyId , down,interval, overall)

#controller manager events
# keyPressed
# keyPressed continuously
# keyRelease
# key toggled
# key mashed
# mouse Moved
# mouse keyPressed
# mouse keyPressed continuously
# mouse keyReleased

class ControllerManager:
    def __init__(self,notifyKeyPressed = None,notifyKeyReleased = None,notifyContinuouslyKeyPressed = None):
        self.notifyContinuouslyKeyPressed = notifyContinuouslyKeyPressed
        self.notifyKeyPressed = notifyKeyPressed
        self.notifyKeyReleased = notifyKeyReleased
	  
     
    def keyChanged(self, key , keyDown):
    #On keyChanged event we update our inner state and notify the subscribers 
        if keyDown:
            if key not in self.inputMap.keys():
                if self.notifyKeyPressed: self.notifyKeyPressed(key)
                self.inputMap[key] = datetime.now();
                self.inputMapIntervals[key] = self.inputMap[key]
        else:
            if key in self.inputMap.keys():
                #self.inputMapReleases[key] = self.inputMap.pop(key)
                
                currTime = datetime.now()

                interval = currTime - self.inputMapIntervals.pop(key)        
                timePassed = currTime - self.inputMap.pop(key)     
                if self.notifyKeyReleased: self.notifyKeyReleased(key,timePassed.total_seconds())
                
        self.notify()
        
        #if self.notifyContinuouslyKeyPressed: self.notifyContinuouslyKeyPressed
    
    def notify(self):
	#iterate over the pressed keys, if it's a new key we notify the subcriber if it's an old key
	#we notify if it was requested
        for key in self.inputMap.keys():

            currTime = datetime.now()
            interval = currTime - self.inputMapIntervals[key]
            
            if self.notifyContinuouslyKeyPressed: 
                didUpdate = self.notifyContinuouslyKeyPressed(key,interval.total_seconds())
                
                
                    

        #for key in self.inputMapReleases.keys():
            

            

    inputMap            = {}; #holds the map of the keys that are pressed and the time they were pressed 
    inputMapIntervals   = {}; #holds the map of the keys that are pressed and the last time we notified about it
    #inputMapReleases    = {}; #holds the map of the keys that were released and still weren't notified


    notifyKeyPressed = None;
    notifyKeyReleased = None;
    notifyContinuouslyKeyPressed = None;

class UnBufferedControllerManager(ControllerManager):
    @logger
    def __init__(self):
         
        super().__init__(self,
            self.notifyKeyPressed,
            self.notifyKeyReleased,
            self.notifyContinuouslyKeyPressed);
    
    def notifyKeyPressed(self,key):
        pass;
    def notifyKeyReleased(self,key):
        pass;
    def notifyContinuouslyKeyPressed(self,key,timePassed):
        pass;
   
class BufferedControllerManager(ControllerManager):
    @logger
    def __init__(self):
         
        super().__init__(self,
            self.notifyKeyPressed,
            self.notifyKeyReleased,
            self.notifyContinuouslyKeyPressed);
    
    def notifyKeyPressed(self,key):
        pass;
    def notifyKeyReleased(self,key):
        pass;
    def notifyContinuouslyKeyPressed(self,key,timePassed):
        pass;
 
def createMask(x):
    retVal = 0L
    for digit in x:
        retVal |= 1 << digit;

    return retVal;

class SubscribtionControllerManager(ControllerManager):
    
    def __init__(self , notifyOnChange = None):
         
        ControllerManager.__init__(self,
            self.notifyKeyPressed,
            self.notifyKeyReleased,
            self.notifyContinuouslyKeyPressed);
    
        self.notifyOnChange = notifyOnChange
    @logger
    def notifyKeyPressed(self,key):
        retVal = False
    	inputBin = createMask(self.inputMap.keys() + [key,])
    	for subscribtion in self.inputSubscribtionMap.keys():
            subsBin = createMask(subscribtion)
    	    keyBin = createMask([key,])
    	    if (inputBin & subsBin) == subsBin and subsBin & keyBin:
                    if self.inputSubscribtionMap[subscribtion]["down"] is True:
                        self.notifyOnChange(self.inputSubscribtionMap[subscribtion]["subscriber"] ,
                                                       self.inputSubscribtionMap[subscribtion]["id"] , 
                                                       down = True)
                        retVal = True
        return retVal
    
    @logger
    def notifyKeyReleased(self,key,timePassed):
        retVal = False
    	inputBin = createMask(self.inputMap.keys() + [key,])
    	for subscribtion in self.inputSubscribtionMap.keys():
            subsBin = createMask(subscribtion)
    	    keyBin = createMask([key,])
    	    if (inputBin & subsBin) == subsBin and subsBin & keyBin:
                    if self.inputSubscribtionMap[subscribtion]["up"] is True:
                        self.notifyOnChange(self.inputSubscribtionMap[subscribtion]["subscriber"] ,
                                                       self.inputSubscribtionMap[subscribtion]["id"] ,  
                                                       down = False ,
						                               overall = timePassed)
                        retVal = True
        return retVal
	
    @logger
    def notifyContinuouslyKeyPressed(self,key,timePassed):
        
        retVal = False
        inputBin = createMask(self.inputMap.keys())
        for subscribtion in self.inputSubscribtionMap.keys():
            subsBin = createMask(subscribtion)
            keyBin = createMask([key,])
            
            if (inputBin & subsBin) == subsBin:
               
                if (self.inputSubscribtionMap[subscribtion]["down"] is True and 
                    timePassed >= self.inputSubscribtionMap[subscribtion]["interval"]) :
                    
                    if ((subsBin in self.subscribtionIntervalsMap and 
                        timePassed - self.subscribtionIntervalsMap[subsBin] >= self.inputSubscribtionMap[subscribtion]["interval"]) or 
                        subsBin not in self.subscribtionIntervalsMap):

                        retVal = True
                        self.subscribtionIntervalsMap[subsBin] = timePassed
                        self.notifyOnChange(self.inputSubscribtionMap[subscribtion]["subscriber"] ,
                                                   self.inputSubscribtionMap[subscribtion]["id"] , 
                                                   down = True , 
                                                   interval = self.inputSubscribtionMap[subscribtion]["interval"] , 
                                                   overall = timePassed)
                        

        return retVal
    
    def subscribeToKey(self, keys , data):
        if keys in self.inputSubscribtionMap:
            self.inputSubscribtionMap[keys].append(data);
        else:
            self.inputSubscribtionMap[keys] = (data);

    inputSubscribtionMap = {};
    subscribtionIntervalsMap = {}
    notifyOnChange = sendControllerSubscribedKeyMsgStub;
    bitMap = 0L

class ControllerSystem(SystemWrapperBase):
    @logger
    def __init__(self):        
       SystemWrapperBase.__init__(self,SystemsExports.getSystemIdByName("ControllerSystem"))
       self.controllerManager = SubscribtionControllerManager(sendControllerSubscribedKeyMsg)
       
       self.registerOperation(MessagesExports.SubscribeToInputMsg.getMessageType(),"onSubscribeToInputMsg");
       self.registerOperation(MessagesExports.CaptureMsg.getMessageType(),"onCaptureMsg");
       self.registerOperation(MessagesExports.ControllerMousePressedMsg.getMessageType(),"onControllerMousePressedMsg");
       self.registerOperation(MessagesExports.ControllerMousePressedMsg.getMessageType(),"onControllerKeyMsg");
    
    @logger   
    def onControllerMousePressedMsg(self,msg):
        
        keyEvent = msg.buttonID
        keyDown = msg.down
        self.controllerManager.keyChanged(keyEvent,keyDown);

    def onControllerKeyMsg(self,msg):
 
        key = msg.getData()
        keyDown = msg.down

        self.controllerManager.keyChanged(key.key,keyDown);

        
    def onControllerMouseMoveMsg(self,msg):
        pass;
    
    def onSubscribeToInputMsg(self,msg):
        
        data = {"id":msg.getData(),
                "down":msg.SubscribtionData.down,
                "up":msg.SubscribtionData.up,
                "interval":msg.SubscribtionData.downInterval,
                "subscriber":msg.SubscribtionData.subscriber}
        tmpKey = ();
        for key in msg.SubscribtionData.keys:
            tmpKey = tmpKey + (key,)
        for mouseKey in msg.SubscribtionData.mouseKeys:
            tmpKey = tmpKey + (mouseKey,)

	
        self.controllerManager.subscribeToKey(tmpKey,data);
    
    def onCaptureMsg(self,msg):        
        self.controllerManager.notify()
        
    controllerManager = None



def main():
    print " "    
    test = SubscribtionControllerManager(sendControllerSubscribedKeyMsgStub)
    
    data = {"id":123,
                "down":True,
                "up":True,
                "interval":0.1,
                "subscriber":10}

    test.subscribeToKey((1,2,3),data)

    '''data = {"id":1,
                "down":True,
                "up":True,
                "interval":0.5,
                "subscriber":10}

    test.subscribeToKey((1,),data)'''

    data = {"id":1,
                "down":True,
                "up":True,
                "interval":0.25,
                "subscriber":10}

    test.subscribeToKey((1,),data)
    
    print "1 pressed"
    test.keyChanged(1,True)
   
    print "2 pressed"
    test.keyChanged(2,True)

    print "3 pressed"
    test.keyChanged(3,True)



    for i in range(1,20):
        print "4 pressed"
        test.keyChanged(4,True)
        time.sleep(0.05)
        print "4 pressed"   
        test.keyChanged(4,False)
        time.sleep(0.05)
    
    print "3 pressed"
    test.keyChanged(3,False)

    print "2 pressed"
    test.keyChanged(2,False)

    print "1 pressed"
    test.keyChanged(1,False)


    

    
if __name__ == "__main__":
    main()
else:
    print(str(inspect.getfile(inspect.currentframe())) + " was parsed");
