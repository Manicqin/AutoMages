import sys
import inspect
import time
import random
from xml.etree import ElementTree
from xml.etree.ElementTree import SubElement
from sqlite3.dbapi2 import Time
import xml.dom.minidom

class ISerialize():
	def __init__(self,val = 0):
		self.val = val

	@property
	def value(self):
		return self.val
	@value.setter
	def value(self , val):
		self.val = val

	def serialize(self , stream,tag = ""):
		if(tag == ""):
			tag = self.__class__.__name__
		sub = ElementTree.Element(tag)		

		for member in inspect.getmembers(self,predicate=lambda x: isinstance(x , ISerialize)):
			if(not member[0].startswith("_")):
				sub = member[1].serialize(sub,member[0])
		stream.append(sub)
		return stream

class serVar(ISerialize):
	def serialize(self , stream,tag=""):
		if(tag == ""):
			tag = self.__class__.__name__
		stream.attrib[tag] = str(self.value)
		return stream
		
class Vector3(ISerialize):
	def __init__(self,x , y , z):
		ISerialize.__init__(self)
		self.x = serVar(x)
		self.y = serVar(y)
		self.z = serVar(z)

class Component(ISerialize):
    def __init__(self):
        ISerialize.__init__(self)

class ScriptComponent(Component):    
	def __init__(self,
					newFileName = None):
		self.FileName = serVar(newFileName)
	FileName = None

class EntityWorldComponent(Component):
	def __init__(self,
					newPosition = None, 
					newOrientation = Vector3(0,0,0), 
					newScale = Vector3(1,1,1)):
		Component.__init__(self)
		self.Position = newPosition
		self.Orientation = newOrientation
		self.Scale = newScale
		
	Position = None
	Orientation = None
	Scale = None


class PhysicsComponent(Component):
	def __init__(self,
					newEnvelope = None,
					newMass = None):
		Component.__init__(self)
		self.Envelope = newEnvelope
		self.Mass = serVar(newMass) if newMass else None
		
	Envelope = None
	Mass = None


class RenderComponent(Component):
	_counter = 0
	def __init__(self,
					newName = None,
					newMesh = None):
		Component.__init__(self)
		self.Name = serVar(newName) if newName else serVar(newMesh+str(RenderComponent._counter)) if newMesh else None
		self.Mesh = serVar(newMesh) if newMesh else None
		RenderComponent._counter = RenderComponent._counter + 1
	Name = None
	Mesh = None

class CameraComponent(Component):
	def __init__(self , 
					newPosition = None, 
					newLookAt = None, 
					newName = None, 
					newNearClipDistance = None , 
					newFarClipDistance = None):
		Component.__init__(self)
		self.Position = newPosition
		self.LookAt = newLookAt
		self.Name = serVar(newName) if newName else None
		self.NearClipDistance = serVar(newNearClipDistance)
		self.FarClipDistance = serVar(newFarClipDistance)
		
	LookAt = None
	Position = None
	Name = None
	NearClipDistance = None
	FarClipDistance = None
	
class Entity(ISerialize):

	def __init__(self,cmpList = []):
		self.ComponentsList = cmpList
		#EntityWorldComponent(),PhysicsComponent(),CameraComponent()
	def serialize(self , stream,tag=""):
		if(tag == ""):
			tag = self.__class__.__name__
		
		ent = ElementTree.Element(tag)
		
		if self.ComponentsList:
			sub = None
			cmps = ElementTree.Element("Components")
			for comp in self.ComponentsList:
				
				cmps = comp.serialize(cmps)
				
			ent.append(cmps)
			stream.append(ent)
		return stream
	
	
	
	ComponentsList = None

class Scene():
	def __init__(self,xml):
		pass

def vector3Generator():
	vector3Generator.retVal.y.value = vector3Generator.retVal.y.value + 10
	return vector3Generator.retVal
vector3Generator.retVal = Vector3(100,100,100)


def vector3RandomGenerator(lambdax = lambda: 0, lambday = lambda: 0, lambdaz = lambda: 0):
	return Vector3(lambdax(),lambday(),lambdaz())
def main():
	filename = "test.xml"
	sceneXml = ElementTree.Element("Scene");
	for count in range(10):
		tmp = lambda: random.randint(0, 500)
		
		sceneXml = Entity([
								EntityWorldComponent(newPosition = vector3RandomGenerator(
																							lambda: random.randint(0, 200),
																							lambda: random.randint(0, 3000),
																							lambda: random.randint(0, 200))),
								PhysicsComponent(newMass = 10),
								RenderComponent(newMesh = "fish.mesh"),
								ScriptComponent(newFileName = "Controls.py")
							]).serialize(sceneXml)	
# 	for count in range(100):
# 		tmp = lambda: random.randint(0, 500)
# 		
# 		sceneXml = Entity([
# 								EntityWorldComponent(newPosition = vector3RandomGenerator(
# 																							lambda: random.randint(0, 200),
# 																							lambda: random.randint(0, 1000),
# 																							lambda: random.randint(0, 200))),
# 								PhysicsComponent(newMass = "0"),
# 								RenderComponent(newMesh = "fish.mesh")
# 							]).serialize(sceneXml)	
	sceneXml = Entity([
						EntityWorldComponent(newPosition = Vector3(200,100,200)),
						CameraComponent(newName="PlayerCam", 
											newNearClipDistance="5" , 
											newLookAt = Vector3(100,0,100),
											newPosition = Vector3(200,100,200),)
						]).serialize(sceneXml)	
	

	if filename:
		ElementTree.ElementTree(sceneXml).write(filename,'')

		prettyFile = open("pretty_"+filename,'w')
		prettyFile.write(xml.dom.minidom.parse(filename).toprettyxml())
		
	else:
		ElementTree.dump(sceneXml)
	
if __name__ == "__main__":
    main()

"""
	<Entity>
		<Components>
			<CameraComponent Name = "PlayerCam" NearClipDistance = "5">
				<Position x="200" y="100" z="200" />
				<LookAt x="20" y="2" z="0" />
			</CameraComponent>
			<EntityWorldComponent>
				<Position x="300" y="200" z="200" />
				<Orientation x="0.00" y="0.00" z="0.00" />
				<Scale x="1.00" y="1.00" z="1.00" />
			</EntityWorldComponent>
			<PhysicsComponent Mass = "10">
				<Envelope x="1.00" y="1.00" z="1.00" />
			</PhysicsComponent>
		</Components>
	</Entity>
"""
