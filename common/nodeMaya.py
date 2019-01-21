# from node import Node
import maya.cmds as cm


class NodeMaya(Node):

 def create_jointsAnimation_MAYA(self, nb_keyFrames):
 	"""
 	Creates the joints structure in MAYA
 	"""
 	cm.joint( name=self.name, p=self.position[0], o=self.rotation[0], roo="zyx", zso=True, oj='zyx', r=True )
 	for child in self.fils:
 		child.create_jointsAnimation_MAYA(nb_keyFrames)
 		cm.select( d=True )
	