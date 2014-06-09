'''
Created on Jan 20, 2013

@author: manicqin
'''
from xml.etree.ElementTree import Element
class   wrapSource:
    mWrappedObj = 0
    
    def __init__(self,wrappedObj):
        self.mWrappedObj = wrappedObj
    
    def getRotation(self):
        if (hasattr(self.mWrappedObj, "rotation_euler")):
            return self.mWrappedObj.rotation_euler
        return self.mWrappedObj.orientation
    
    def getLocation(self):
        return self.mWrappedObj.location
    
    def getScale(self):
        return self.mWrappedObj.scale
    
    
class vector3:
    x = 0 
    y = 0 
    z = 0
    
class   entityInfo:
            
    orientation =   vector3();
    location    =   vector3();
    scale       =   vector3();
    name        =   ""
    mesh        =   "Cube.mesh"
    mass        =   10
    
frmtPrec = "%.2f";
def createEntWrldCmp(obj):
    EntityWorldComponent = Element('EntityWorldComponent')
    Orientation = Element('Orientation')
    Orientation.set('x',frmtPrec % obj.rotation_euler.x)
    Orientation.set('y',frmtPrec % obj.rotation_euler.y)
    Orientation.set('z',frmtPrec % obj.rotation_euler.z)

    Position = Element('Position')
    Position.set('x',frmtPrec % (obj.location.x))
    Position.set('y',frmtPrec % (obj.location.y))
    Position.set('z',frmtPrec % (obj.location.z))

    Scale = Element('Scale')
    Scale.set('x',frmtPrec % obj.scale.x)
    Scale.set('y',frmtPrec % obj.scale.y)
    Scale.set('z',frmtPrec % obj.scale.z)

    EntityWorldComponent.append(Position)
    EntityWorldComponent.append(Orientation)
    EntityWorldComponent.append(Scale)
    
    return EntityWorldComponent

def createRndrCmp(obj):
    RenderComponent = Element('RenderComponent')
    Name = Element('Name')
    Name.text = obj.name
    
    Mesh = Element('Mesh')
    Mesh.text = obj.mesh
    
    RenderComponent.append(Name)
    RenderComponent.append(Mesh)
    return RenderComponent

def createPhysCmp(obj):
    PhysicsComponent = Element('PhysicsComponent')
    Mass = Element('Mass')
    Mass.text = str(obj.mass)
    
    PhysicsComponent.append(Mass)
    return PhysicsComponent

Scene = Element('Scene')
Entity = Element('Entity')
Components = Element('Components')
obj = entityInfo();
Components.append(createEntWrldCmp(obj))
Components.append(createRndrCmp(obj))
Components.append(createPhysCmp(obj))    
Entity.append(Components)
Scene.append(Entity)