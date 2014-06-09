import SystemsExports;
import MessagesExports;
import ComponentsExports;
import Tkinter as tk

from xml.etree import ElementTree
from xml.etree.ElementTree import SubElement

from entities import *;

def createEntity():
    #channel2 = SystemsExports.createChannelHandler("initChannel");

    cmpntSysId  = SystemsExports.getSystemIdByName("ComponentsSystem")
    sceneXml = ElementTree.Element("Scene")
    sceneXml = Entity([
                                EntityWorldComponent(newPosition = wrapVector3([100,100,100])),
                                PhysicsComponent(newMass = 10),
                                RenderComponent(newMesh = "fish.mesh")
                            ]).serialize(sceneXml)
    if(cmpntSysId):

        initMsg = MessagesExports.CreateSceneMsg.createFromData(ElementTree.tostring(sceneXml))
        #cmInit = MessagesExports.ChanneledMessages.createChanneledMsg(channel2,initMsg.createSharedPtr());
        SystemsExports.getSystemChannelIn(cmpntSysId).addMsg(initMsg.createSharedPtr());



tk._default_root = tk.Tk(baseName = "")

class Application(tk.Frame):
    def __init__(self, master=None):
        tk.Frame.__init__(self, master)
        self.grid()
        self.createWidgets()

    def createWidgets(self):
        self.addButton = tk.Button(self, text='add', command=createEntity)
        self.removeButton = tk.Button(self, text='remove', command=createEntity)

        self.addButton.grid()
        self.removeButton.grid()


app = Application()
app.master.title('Sample application')
app.mainloop()
print ("GUI ENDED")
