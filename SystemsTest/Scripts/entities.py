import sys
import inspect
from datetime import datetime
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
		if self.value is not None:
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
					newMass = None,
					newGravity = None):
		Component.__init__(self)
		self.Envelope = newEnvelope
		self.Mass = serVar(newMass) if newMass else None
		self.Gravity = newGravity
	Envelope = None
	Mass = None
	Gravity = None

class RenderComponent(Component):
	_counter = 0
	def __init__(self,
					newName = None,
					newMesh = None):
		Component.__init__(self)
		self.Name = serVar(newName) if newName else serVar(newMesh+str(datetime.now().second)+":"+str(datetime.now().microsecond)) if newMesh else None 
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
	def serialize(self , stream=None,tag=""):
		if(tag == ""):
			tag = self.__class__.__name__
		
		ent = ElementTree.Element(tag)
		
		if self.ComponentsList:
			sub = None
			cmps = ElementTree.Element("Components")
			for comp in self.ComponentsList:
				
				cmps = comp.serialize(cmps)
				
			ent.append(cmps)
			
			if stream is not None:
				stream.append(ent)
			else:
				stream = ent
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

#get the nearest power of 3 (ceiling)
#divide the cube by the log(power)
#randomize 1:power
def nearesetPower(val):
	retVal = 2;
	for count in range(2,50):
		if count**3 > val:
			retVal = count
			break;
	return retVal


def cubeRandomizer(cornerVector3  , edgeLength , itemsCount):
	x = 0;
	y = 0;
	z = 0;
	devider = nearesetPower(itemsCount);
	segLength = edgeLength / devider
	print (edgeLength , devider , segLength)
	for iDx in random.sample(range(devider**3), itemsCount):
		z = iDx % devider;
		y = (iDx / devider) % devider;
		x = iDx / (devider*devider);
		#print(iDx , (x +0.5)* segLength + cornerVector3[0], (y +0.5)* segLength + cornerVector3[0], (z +0.5)* segLength + cornerVector3[0])
		yield ((x +0.5)* segLength + cornerVector3[0], (y +0.5)* segLength + cornerVector3[0], (z +0.5)* segLength + cornerVector3[0]) 

def wrapVector3(val):
	
	return Vector3(val[0],val[1],val[2])

def createScene(filename):
	sceneXml = ElementTree.Element("Scene");
	for count in cubeRandomizer((100,100,100),edgeLength =  50,itemsCount = 1):
		
		
		sceneXml = Entity([
								EntityWorldComponent(newPosition = wrapVector3(count)),
								PhysicsComponent(newMass = 10),
								RenderComponent(newMesh = "fish.mesh")
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
						EntityWorldComponent(newPosition = Vector3(300,300,500)),
						CameraComponent(newName="DefaultCamera", 
											newNearClipDistance="5" , 
											newLookAt = Vector3(100,0,100),
											newPosition = Vector3(500,300,500)),
						PhysicsComponent(newMass = 10, newGravity= Vector3(0,0,0))
						]).serialize(sceneXml)	
	

	if filename:
		completePath = "../../Configurations/Scenes/"+filename
		ElementTree.ElementTree(sceneXml).write(completePath,'')

# 		prettyFile = open("../../Configurations/Scenes/pretty_"+filename,'w')
# 		prettyFile.write(xml.dom.minidom.parse(completePath).toprettyxml())
		
	else:
		ElementTree.dump(sceneXml)

def main():
	createScene("test.xml")
	
if __name__ == "__main__":
    main()
