


'''
class SystemWrapper(SystemWrapperBase):
    @logger
    def __init__(self):
                
        pass
    def capture(self):
        
        print (self.inputMap , self.inputMapIntervals , self.inputSubscribtionMap)
        if(len(self.inputMap) or len(self.inputMapIntervals)):
           # print "inputMap",self.inputMap
           # Check if the current key map equals to the buffered key map        
           for key in self.inputMap:
                print key , self.inputMapIntervals.keys()
                # If we find a key that is in the current key map but not in the buffered then it means a new key was pressed 
                if key not in self.inputMapIntervals.keys():
                     
                    for entrance in self.inputSubscribtionMap:
                        print entrance
                        if key in entrance:
                            
                            if self.inputSubscribtionMap[entrance]["down"] is True:
                                print self.inputSubscribtionMap[entrance]["down"]
                                self.inputMapIntervals[key] = self.inputMap[key]
                                sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                               self.inputSubscribtionMap[entrance]["id"], 
                                                               down = True)
                               
                               
                # If the key is not new then we need to check if we need to send a msg about how long was it pressed
                else:
                    currTime = datetime.now()
                    timePassed = currTime - self.inputMapIntervals[key]                    
                    for entrance in self.inputSubscribtionMap:
                        if key in self.inputSubscribtionMap[entrance]:
                            if (self.inputSubscribtionMap[entrance]["down"] is True and 
                                timePassed.total_seconds() > self.inputSubscribtionMap[entrance]["interval"]) :
                                print self.inputSubscribtionMap[entrance]["interval"]
                                overall = currTime - self.inputMap[key]
                                sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                               self.inputSubscribtionMap[entrance]["id"], 
                                                               down = True , 
                                                               interval = timePassed.total_seconds() , 
                                                               overall = overall.total_seconds())

                    
                    self.inputMapIntervals[key] = currTime
                 # Check if the buffered key map equals to the current key map           
                for key in self.inputMapIntervals.keys():
                
                    # if There is a key missing from the current key map then a key was released
                    if key not in self.inputMap.keys():
                        if self.inputSubscribtionMap[entrance]["up"] is True:
                            
                            interval = self.inputMapIntervals.pop(key)
                            sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                           self.inputSubscribtionMap[entrance]["id"], 
                                                           down = False , 
                                                           interval = 0.0 , 
                                                           overall = interval.second)
                            
                            
            
        #self.inputMap.clear()

    def onCaptureMsg(self,msg):        
        self.capture()
        
    def onControllerMousePressedMsg(self,msg):
        
        keyEvent = msg.buttonID
        keyDown = msg.down
#         print keyDown,keyEvent,self.inputMap.values()
        
        if keyDown:
            if keyEvent not in self.inputMap: 
                self.inputMap[keyEvent] = datetime.now();
        else:
            if keyEvent in self.inputMap:
                interval = self.inputMap.pop(keyEvent)
        
        self.capture()
    def onControllerKeyMsg(self,msg):
 
        keyEvent = msg.getData()
        keyDown = msg.down
        
        #print self.inputMap.values()
        if keyDown:
            if keyEvent.key not in self.inputMap: 
                self.inputMap[keyEvent.key] = datetime.now();
        else:
            if keyEvent.key in self.inputMap:
                interval = self.inputMap.pop(keyEvent.key)
                #print keyEvent.key , time.time() - interval;
            
        self.capture()
    def onControllerMouseMoveMsg(self,msg):
        #print MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()),str(inspect.getlineno(inspect.currentframe())),"members:",inspect.getmembers(msg)
        self.capture()
        
        
        data = {"id":msg.getData(),
                "down":msg.SubscribtionData.down,
                "up":msg.SubscribtionData.up,
                "downInterval":msg.SubscribtionData.downInterval,
                "subscriber":msg.SubscribtionData.subscriber}
        self.inputSubscribtionMap[msg.SubscribtionData.keys] = data;
        print data
    
    inputMap = {};
    inputMapIntervals = {};
    
    inputSubscribtionMap = {};
    
    interval = 0.5;
'''





'''
class SystemWrapper(SystemWrapperBase):
    @logger
    def __init__(self):
                
        pass
    def capture(self):
        
        print (self.inputMap , self.inputMapIntervals , self.inputSubscribtionMap)
        if(len(self.inputMap) or len(self.inputMapIntervals)):
           # print "inputMap",self.inputMap
           # Check if the current key map equals to the buffered key map        
           for key in self.inputMap:
                print key , self.inputMapIntervals.keys()
                # If we find a key that is in the current key map but not in the buffered then it means a new key was pressed 
                if key not in self.inputMapIntervals.keys():
                     
                    for entrance in self.inputSubscribtionMap:
                        print entrance
                        if key in entrance:
                            
                            if self.inputSubscribtionMap[entrance]["down"] is True:
                                print self.inputSubscribtionMap[entrance]["down"]
                                self.inputMapIntervals[key] = self.inputMap[key]
                                sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                               self.inputSubscribtionMap[entrance]["id"], 
                                                               down = True)
                               
                               
                # If the key is not new then we need to check if we need to send a msg about how long was it pressed
                else:
                    currTime = datetime.now()
                    timePassed = currTime - self.inputMapIntervals[key]                    
                    for entrance in self.inputSubscribtionMap:
                        if key in self.inputSubscribtionMap[entrance]:
                            if (self.inputSubscribtionMap[entrance]["down"] is True and 
                                timePassed.total_seconds() > self.inputSubscribtionMap[entrance]["interval"]) :
                                print self.inputSubscribtionMap[entrance]["interval"]
                                overall = currTime - self.inputMap[key]
                                sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                               self.inputSubscribtionMap[entrance]["id"], 
                                                               down = True , 
                                                               interval = timePassed.total_seconds() , 
                                                               overall = overall.total_seconds())

                    
                    self.inputMapIntervals[key] = currTime
                 # Check if the buffered key map equals to the current key map           
                for key in self.inputMapIntervals.keys():
                
                    # if There is a key missing from the current key map then a key was released
                    if key not in self.inputMap.keys():
                        if self.inputSubscribtionMap[entrance]["up"] is True:
                            
                            interval = self.inputMapIntervals.pop(key)
                            sendControllerSubscribedKeyMsg(self.inputSubscribtionMap[entrance]["subscriber"],
                                                           self.inputSubscribtionMap[entrance]["id"], 
                                                           down = False , 
                                                           interval = 0.0 , 
                                                           overall = interval.second)
                            
                            
            
        #self.inputMap.clear()

    def onCaptureMsg(self,msg):        
        self.capture()
        
    def onControllerMousePressedMsg(self,msg):
        
        keyEvent = msg.buttonID
        keyDown = msg.down
#         print keyDown,keyEvent,self.inputMap.values()
        
        if keyDown:
            if keyEvent not in self.inputMap: 
                self.inputMap[keyEvent] = datetime.now();
        else:
            if keyEvent in self.inputMap:
                interval = self.inputMap.pop(keyEvent)
        
        self.capture()
    def onControllerKeyMsg(self,msg):
 
        keyEvent = msg.getData()
        keyDown = msg.down
        
        #print self.inputMap.values()
        if keyDown:
            if keyEvent.key not in self.inputMap: 
                self.inputMap[keyEvent.key] = datetime.now();
        else:
            if keyEvent.key in self.inputMap:
                interval = self.inputMap.pop(keyEvent.key)
                #print keyEvent.key , time.time() - interval;
            
        self.capture()
    def onControllerMouseMoveMsg(self,msg):
        #print MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()),str(inspect.getlineno(inspect.currentframe())),"members:",inspect.getmembers(msg)
        self.capture()
        
        
        data = {"id":msg.getData(),
                "down":msg.SubscribtionData.down,
                "up":msg.SubscribtionData.up,
                "downInterval":msg.SubscribtionData.downInterval,
                "subscriber":msg.SubscribtionData.subscriber}
        self.inputSubscribtionMap[msg.SubscribtionData.keys] = data;
        print data
    
    inputMap = {};
    inputMapIntervals = {};
    
    inputSubscribtionMap = {};
    
    interval = 0.5;
'''

'''class SubscribtionControllerManagerOld(ControllerManager):
    
    def __init__(self , notifyOnChange = None):
         
        ControllerManager.__init__(self,
            self.notifyKeyPressed,
            self.notifyKeyReleased,
            self.notifyContinuouslyKeyPressed);
    
        self.notifyOnChange = notifyOnChange

    @logger
    def notifyKeyPressed(self,key):
        for entrance in self.inputSubscribtionMap.keys():
            for r in range(1,4):
                combinations = itertools.combinations(self.inputMap.keys(),r)
                for comb in combinations:
                    if entrance == comb:
                        print ("notifyKeyPressed == comb: " , comb , " entrance: ", entrance)
                        if self.inputSubscribtionMap[entrance]["down"] is True:
                            self.notifyOnChange(self.inputSubscribtionMap[entrance]["subscriber"],
                                                           self.inputSubscribtionMap[entrance]["id"], 
                                                           down = True)
    
    @logger
    def notifyKeyReleased(self,key,timePassed):
        if True:
        for r in range(1,4):
                combinations = itertools.combinations(self.inputMap.keys(),r)
                for comb in combinations:
                    if key in comb:
                        print ("notifyKeyReleased == comb: " , comb , " entrance: ", key )
                        if self.inputSubscribtionMap[entrance]["up"] is True:
                            
                            self.notifyOnChange(self.inputSubscribtionMap[entrance]["subscriber"],
                                                           self.inputSubscribtionMap[entrance]["id"], 
                                                           down = False , 
                                                           interval = 0.0 , 
                                                            overall = timePassed)

    @logger
    def notifyContinuouslyKeyPressed(self,key,timePassed):
        for entrance in self.inputSubscribtionMap.keys():
            for r in range(1,4):
                combinations = itertools.combinations(self.inputMap.keys(),r)
                for comb in combinations:
                    if entrance == comb:
                        print ("notifyContinuouslyKeyPressed == comb: " , comb , " entrance: ", entrance)
                        if (self.inputSubscribtionMap[entrance]["down"] is True and 
                        timePassed >= self.inputSubscribtionMap[entrance]["interval"]) :

                            self.notifyOnChange(self.inputSubscribtionMap[entrance]["subscriber"],
                                                           self.inputSubscribtionMap[entrance]["id"], 
                                                           down = True , 
                                                           interval = self.inputSubscribtionMap[entrance]["interval"] , 
                                                           overall = timePassed)
    
    def subscribeToKey(self, keys , data):
        if keys in self.inputSubscribtionMap:
            self.inputSubscribtionMap[keys].append(data);
        else:
            self.inputSubscribtionMap[keys] = (data);
    
    inputSubscribtionMap = {};
    notifyOnChange = sendControllerSubscribedKeyMsgStub;'''