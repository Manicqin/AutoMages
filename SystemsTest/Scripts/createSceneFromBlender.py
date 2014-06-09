from xml.etree.ElementTree import Element,SubElement,Comment,ElementTree
import bpy


frmtPrec = "%.2f";
def createEntWrldCmp(obj):
    EntityWorldComponent = Element('EntityWorldComponent')
    Orientation = Element('Orientation')
    Orientation.set('x',frmtPrec % obj.rotation_euler.x)
    Orientation.set('y',frmtPrec % obj.rotation_euler.y)
    Orientation.set('z',frmtPrec % obj.rotation_euler.z)

    Position = Element('Position')
    Position.set('x',frmtPrec % (obj.location.x * bpy.data.scenes[0].unit_settings.scale_length))
    Position.set('y',frmtPrec % (obj.location.y * bpy.data.scenes[0].unit_settings.scale_length))
    Position.set('z',frmtPrec % (obj.location.z * bpy.data.scenes[0].unit_settings.scale_length))

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
    Mesh.text = 'Cube.mesh'
    
    RenderComponent.append(Name)
    RenderComponent.append(Mesh)
    return RenderComponent

def createPhysCmp(obj):
    PhysicsComponent = Element('PhysicsComponent')
    Mass = Element('Mass')
    Mass.text = str(10)
    
    PhysicsComponent.append(Mass)
    return PhysicsComponent

Scene = Element('Scene')
for obj in bpy.data.objects:
    Entity = Element('Entity')
    Components = Element('Components')
     
    Components.append(createEntWrldCmp(obj))
    Components.append(createRndrCmp(obj))
    Components.append(createPhysCmp(obj))    
    Entity.append(Components)
    Scene.append(Entity)
    
f = open('/home/manicqin/Documents/out.xml','wb')
ElementTree(Scene).write(f, method='xml')
f.close()
