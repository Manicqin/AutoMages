'''
Created on Jan 28, 2013

@author: manicqin
'''

import SystemsExports;
import MessagesExports;

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

class SystemWrapper:
    def __init__(self):
        SystemsExports.LogFunc("SystemWrapper init");
    
    def test2(self,msg):
        printData(msg)

def test():
    SystemsExports.LogFunc("test Called from Controls.py");
    
SystemsExports.LogFunc("Controls.py was parsed");