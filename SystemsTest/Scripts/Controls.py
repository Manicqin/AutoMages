'''
Created on Jan 28, 2013

@author: manicqin
'''
import SystemsExports;
import OISExports;
import MessagesExports;

from SystemWrapperBase import *;
import entities
import createEntityOD

import inspect
import time;

def printData(msg):
    if  (msg.getItemTypeId() == MessagesExports.StringDataMsg.getItemTypeId() or
         msg.getItemTypeId() == MessagesExports.FloatDataMsg.getItemTypeId() or
         msg.getItemTypeId() == MessagesExports.IntDataMsg.getItemTypeId()):
            SystemsExports.LogFunc(str(msg.getData()) + " " + MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()) + " " + str(msg.getItemTypeId()));
    elif (msg.getItemTypeId() == MessagesExports.StringComposerMsg.getItemTypeId()):
            SystemsExports.LogFunc(MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()) + " " + str(msg.getItemTypeId()));
            for txt in msg.getData():
                SystemsExports.LogFunc("    " + txt);
    else:
         SystemsExports.LogFunc(MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()) + " " + str(msg.getItemTypeId()));

def whatButtonsPressed(buttons):
    print "Buttons pressed: ",
    if OISExports.buttonDown(buttons,OISExports.MB_Left):
        print OISExports.MB_Left ,
    if OISExports.buttonDown(buttons,OISExports.MB_Right):
        print OISExports.MB_Right ,
    if OISExports.buttonDown(buttons,OISExports.MB_Middle):
        print OISExports.MB_Middle ,
    if OISExports.buttonDown(buttons,OISExports.MB_Button3):
        print OISExports.MB_Button3 ,
    if OISExports.buttonDown(buttons,OISExports.MB_Button4):
        print OISExports.MB_Button4 ,
    if OISExports.buttonDown(buttons,OISExports.MB_Button5):
        print OISExports.MB_Button5 ,
    if OISExports.buttonDown(buttons,OISExports.MB_Button6):
        print OISExports.MB_Button6 ,
    if OISExports.buttonDown(buttons,OISExports.MB_Button8):
        print OISExports.MB_Button8 ,
    print "."

def subscribeToKey(subscriber , eventId , down = False , up = False , interval  = 0.0 ,  mouseKeys = None , keys = None):
    subscribeData = MessagesExports.KeySubscribtion()
    subscribeData.down = down
    subscribeData.up = up
    subscribeData.interval = interval
    subscribeData.subscriber = subscriber;
    if mouseKeys: subscribeData.mouseKeys.append(mouseKeys)
    if keys: subscribeData.keys.append(keys)

    msg = MessagesExports.SubscribeToInputMsg(eventId)
    msg.SubscribtionData = subscribeData
    SystemsExports.sendMsg(msg.createSharedPtr())

class OgreSystem(SystemWrapperBase):
    @logger
    def __init__(self):

        SystemWrapperBase.__init__(self,SystemsExports.getSystemIdByName("OgreSystem"))
        
        self.registerOperation(MessagesExports.ControllerMousePressedMsg.getMessageType(),"onControllerMousePressedMsg");
        #self.registerOperation(MessagesExports.ControllerSubscribedKeyMsg.getMessageType(),"onControllerSubscribedKeyMsg");
	    
        #subscribeToKey(self.getSystemId() , 10 , down=True, up =True, interval=0.1 , mouseKeys = OISExports.MB_Left);
        #subscribeToKey(self.getSystemId() , 20 , down=True, up =True, interval=0.1 , keys = OISExports.KC_W);
        #subscribeToKey(self.getSystemId() , 21 , down=True, up =True, interval=0.1 , keys = OISExports.KC_A);
        #subscribeToKey(self.getSystemId() , 22 , down=True, up =True, interval=0.1 , keys = OISExports.KC_D);
        #subscribeToKey(self.getSystemId() , 23 , down=True, up =True, interval=0.1 , keys = OISExports.KC_S);

    def onControllerMousePressedMsg(self,msg):

        data = msg.getData()
        self.buttons = data.buttons

        if self.getSystemId() and msg.buttonID == OISExports.MB_Left and msg.down == False:

	    print("before");
            channel2 = SystemsExports.createChannelHandler("CastRayMsgChannel");
            CastRayMsg = MessagesExports.CastRayMsg(data.X.abs,data.Y.abs)
            SystemsExports.getSystemChannelIn(self.getSystemId()).addMsg(MessagesExports.ChanneledMessages.createChanneledMsg(channel2,CastRayMsg.createSharedPtr()));
            SystemsExports.waitOnChannel(channel2,10,0,1);
            retMsg = SystemsExports.getMsgFromChannel(channel2); 
            print("after");




            if retMsg.getMessageType() == MessagesExports.FocusEntity.getMessageType() :
                print 'FocusEntity ' , retMsg.getData()

	    if retMsg.getMessageType() == MessagesExports.VectorMsg.getMessageType() :
                point = retMsg.getData();
		#print 'VectorMsg ' , point.x , point.y , point.z
		createEntityOD.createEntityInPlace(point.x , point.y , point.z)
	    print("after2");
        if self.getSystemId() and msg.buttonID == OISExports.MB_Right and msg.down == False:
            print (data.X.abs,data.Y.abs,data.Z.abs)
            #createEntityOD.createEntity(entities.Vector3(100,100,100))
            
    @logger
    def onControllerSubscribedKeyMsg(self,msg):
        keyEvent = msg.getData()
        isDown = msg.down
        interval = msg.interval
        overall = msg.overall
        
        x = 0
        y = 0
        z = 0
        factor = 30
        power = 100
        entityId = 7
        
        power = power - (factor * overall)
        print keyEvent
        if power < 0: power = 0
        if keyEvent is 20 and isDown:
            y = 0 + power
        if keyEvent is 21 and isDown:
	       x = 0 - power
        if keyEvent is 22 and isDown:
            x = 0 + power
        if keyEvent is 23 and isDown:
	       y = 0 - power
        if keyEvent is 10 and isDown:
	       print("Mouse")
			
        force = MessagesExports.ApplyImpulseMsg.create(x,y,z)
        force.entityId = entityId
        SystemsExports.sendMsg(force.createSharedPtr())

        #else:
        #    force = MessagesExports.ClearForces(entityId)
        #    SystemsExports.sendMsg(force.createSharedPtr())

        LinearVelocity = SystemsExports.getPhysicsSystem().getLinearVelocityForEntity(entityId);
        force = SystemsExports.getPhysicsSystem().getForceForEntity(entityId);
    
    def onControllerBufferedKeyMsg(self,msg):
       # msg.__class__ = MessagesExports.ControllerKeyMsg
        #print MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()),str(inspect.getfile(inspect.currentframe())),"members:",inspect.getmembers(msg)
        keyEvent = msg.getData()
        keyDown = msg.down

        #print time.gmtime()
        x = 0
        y = 0
        z = 0
        factor = 500
        power = 0
        entityId = 7
        if keyDown:
            power = factor * msg.overall
            #print msg.overall , power
            if keyDown and keyEvent is OISExports.KC_A:
                x = 0-power

            if keyDown and keyEvent is OISExports.KC_D:
                x = power

            if keyDown and keyEvent is OISExports.KC_W:
                y = power

            if keyDown and keyEvent is OISExports.KC_S:
                y = 0-power

            if keyDown and keyEvent is OISExports.KC_Q:
                z = 0-power

            if keyDown and keyEvent is OISExports.KC_E:
                z = power
            #print x,y,z
            force = MessagesExports.ApplyForceMsg.create(x,y,z)
            force.entityId = entityId
            SystemsExports.sendMsg(force.createSharedPtr())

        else:
            force = MessagesExports.ClearForces(entityId)
            SystemsExports.sendMsg(force.createSharedPtr())

        LinearVelocity = SystemsExports.getPhysicsSystem().getLinearVelocityForEntity(entityId);
        force = SystemsExports.getPhysicsSystem().getForceForEntity(entityId);

        #print (LinearVelocity.x , LinearVelocity.y , LinearVelocity.z)
        #print (force.x , force.y , force.z)
    @logger
    def onCastRayMsg(self,msg):
        #print MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()),str(inspect.getfile(inspect.currentframe())),"members:",inspect.getmembers(msg)
        pass

    @logger
    def test(self,msg):
        print "TEST";
    buttons = 0;


SystemsExports.LogFunc(str(inspect.getfile(inspect.currentframe())) + " was parsed");
