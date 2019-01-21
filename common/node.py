"""
Hierarchy model of the class
"""
import maya.cmds as cm


GLOBAL_TAB_COUNT = 0

class Node:

	def __init__(self, name, parent = None):
		self.parent = parent
		self.name = name
		self.fils = []
		self.translate = []
		self.rotate = []
		self.position = []
		self.frame_time = 0

	@staticmethod
	def readInfo(f, info_frame, end=False):
		"""
		Returns the info from the file in the form of a list.
		Also checks if the file as the expected behaviour.

		Parameters
		----------
		f				List of strings
						Lines of the file

		info_frame		List of floats
						Frame info for every rotation/position information,
						those information are stored at the end of the bvh file

		end 			boolean
						Should be true for an "End" Node

		Returns
		-------
		offset, position, rotation
			List of floats containing the information from the file
		"""

		#### Lire l'accolade #####
		l = Node.readline(f)
		if l[0] != "{":
			print("Invalid input")
			raise

		#### OFFSET ####
		l = Node.readline(f)
		if l[0] != "OFFSET":
			print("Invalid input")
			raise
		offset = [float(l[o]) for o in range(1, 4)]

		if end is True:
			return offset

		#### CHANNELS ###
		l = Node.readline(f)
		if l[0] != "CHANNELS":
			print("Invalid input")
			raise

		number_channels = int(l[1])
		rotation = []
		position = []

		for frame in range(len(info_frame[0])):
			if number_channels == 6: #A CHANGER ICI 
				positionLoc = [info_frame[0][frame], info_frame[1][frame], info_frame[2][frame]]
				position.append(positionLoc)
				rotationLoc = [info_frame[0][frame], info_frame[1][frame], info_frame[2][frame]]
				rotation.append(rotationLoc)
			else:
				rotationLoc = [info_frame[0][frame], info_frame[1][frame], info_frame[2][frame]]
				rotation.append(rotationLoc)
		for i in range(number_channels):
			info_frame.pop(0)
		return offset, position, rotation

	@staticmethod
	def CreateChild(parent, f, info_frame):
		"""
		Create a Node child

		Parameters
		----------
		parent			Node object
						current node we are searching childs for

		f				List of strings
						Lines of the file

		info_frame		List of floats
						Frame info for every rotation/position information,
						those information are stored at the end of the bvh file

		Returns
		-------
		None
			When used well, user should input the root node
			to this function and we get the "filled" root node
			in the end
		"""
		if (len(f) != 0):
			l = Node.readline(f)
			while l[0] != "MOTION":
				if l[0] == "JOINT":
					Child = Node(l[1], parent)
					offset, position, rotation = Node.readInfo(f, info_frame)
					Child.translate = offset; Child.position = position; Child.rotate = rotation
					parent.fils.append(Child)
					parent = Child
				elif l[0] == "End":
					Child = Node(l[1], parent)
					offset = Node.readInfo(f, info_frame, True)
					Child.translate = offset
					parent.fils.append(Child)
					parent = Child
				elif l[0] == "}":
					parent = parent.parent
				else:
					print("Invalid input")
					raise
				l = Node.readline(f)

	@staticmethod
	def readline(f):
		"""
		Reads every line of the file since BVH are generated by
		Windows, a simple read line was not working

		Parameters
		----------
		f 			list of strings / list of line of a file
					The list of lines of a file,
					should be cleaned before by :
					with open(file_name) as f:
						f = f.read().strip()
						f = f.split("\n")motion_info
						line = Node.readline(f)

		Returns
		-------
		list of strings
			first line of the file splitted of " "

		"""
		l = f[0].replace("\t", "")
		l = l.replace("\r", "")
		f.pop(0)
		while len(l) == 0:
			l = f[0].replace("\t", "")
			f.pop(0)

		l = l.split(" ")
		return l

	@staticmethod
	def createStructureFromBVH(file_name):
		"""
		Creates the BVH structure

		Parameters
		----------
		file_name 		string
						name of the file : needs to be a .bvh

		Returns
		-------
		Node object
			Root node of the tree
		"""

		assert file_name[-4:] == ".bvh"

		Root = Node(None)

		info_frame = []

		with open(file_name) as motion_info:
			f = motion_info.read().strip()
			f = f.splitlines()

			line = Node.readline(f)

			while (Node.readline(f)[0] != "MOTION"):
				continue

			print("ROOT NODE CREATED")
			number_of_frames = int(Node.readline(f)[1])
			Root.frame_time = float(Node.readline(f)[2])

			info_frame = [[float(j) for j in i.split()] for i in f]
			tmp = []
			for nodeID in range(len(info_frame[0])):
				deviceFrames = []
				for frame in range(len(info_frame)):
					deviceFrames.append(info_frame[frame][nodeID])
				tmp.append(deviceFrames)
			info_frame = tmp


				


		with open(file_name) as f: # Use file to refer to the file object
			f = f.read().strip()
			f = f.split("\n")
			line = Node.readline(f)

			while (line[0] != "ROOT"):
				line = Node.readline(f)
				if line is None: # On n'a pas trouvé la root dans le fichier
					return Root

			Root.name = line[1]
			offset, position, rotation = Node.readInfo(f, info_frame)
			Root.translate = offset; Root.position = position; Root.rotate = rotation

			Node.CreateChild(Root, f, info_frame);

			# We need to assert that info_frame is fully empty
			for i in info_frame:
				assert len(i) == 0

			return Root


	def create_jointsAnimation_MAYA(self, nb_keyFrames):
	 	"""
	 	Creates the joints structure in MAYA
	 	"""
		positionM = []
		rotationM = []
		if len(self.position) != 0:
			positionM = self.position[0]
		else:
			positionM = [0, 0, 0]
		if len(self.rotate) != 0:
			rotationM = self.rotate[0]
		else:
			rotationM = [0, 0, 0]
		#print(positionM)

	 	cm.joint( name=self.name, p=positionM, o=rotationM, roo="zyx", zso=True, oj='zyx', r=True )
	 	for child in self.fils:
	 		child.create_jointsAnimation_MAYA(nb_keyFrames)
	 		cm.select( d=True )



	def __str__(self):
		global GLOBAL_TAB_COUNT
		"""
		Stringify method of the object
		Should return the same output as the bvh file (the part before motion)

		The bvh file and the output from this method should be the same
		for the root node (without tabs)
		"""
		string = ""
		if (len(self.fils) != 0):
			string += "   " * GLOBAL_TAB_COUNT + "JOINT " + self.name + "\n"
		else:
			string += "   " * GLOBAL_TAB_COUNT + "END " + self.name + "\n"
		string += "   " * GLOBAL_TAB_COUNT + "{\n"

		GLOBAL_TAB_COUNT += 1

		string += "   " * GLOBAL_TAB_COUNT + "OFFSET " + " ".join([str(i) for i in self.translate]) + "\n"
		if len(self.position) + len(self.rotate) != 0:
			string += "   " * GLOBAL_TAB_COUNT + "CHANNELS " + str(len(self.position) + len(self.rotate)) + " "
			if len(self.position) != 0:
				string += " ".join([(str(i[0]) + "position") for i in self.position]) + " "
			if len(self.rotate) != 0:
				string += " ".join([(str(i[0]) + "rotation") for i in self.rotate])
			string += "\n"

		for i in self.fils:
			child_str = i.__str__()
			if child_str is not None:
				string += child_str

		GLOBAL_TAB_COUNT -= 1

		string += "   " * GLOBAL_TAB_COUNT + "}\n"

		return string
