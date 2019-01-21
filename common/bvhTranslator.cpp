#include "bvhTranslator.h"

///Creates one instance of the BvhTranslator
void* BvhTranslator::creator()
{
	return new BvhTranslator();
}

// Initialize our magic string
MString const BvhTranslator::magic("HIERARCHY");

/// An .bvh file is an ascii whose first line contains the string HIERARCHY.
/// We use the .bvh parser implemented in Joint class (from SIA project)
///
MStatus BvhTranslator::reader(const MFileObject& file,
	const MString& options,
	MPxFileTranslator::FileAccessMode mode)
{
	const MString fname = file.fullName();

	MStatus rval(MS::kSuccess);
	const int maxLineSize = 1024;
	char buf[maxLineSize];

	ifstream inputfile(fname.asChar(), ios::in);
	if (!inputfile) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}

	if (!inputfile.getline(buf, maxLineSize)) {
		cerr << "file " << fname << " contained no lines ... aborting\n";
		return MS::kFailure;
	}

	if (0 != strncmp(buf, magic.asChar(), magic.length())) {
		cerr << "first line of file " << fname;
		cerr << " did not contain " << magic.asChar() << " ... aborting\n";
		return MS::kFailure;
	}
	inputfile.close();

	/// reading .bvh file with Joint parser
	root = Joint.createFromFile(fname.asChar());
	
	/// creating MFnIkJoint and MFnAnimCurve for animation
	map<string, bool> visited;
	createMFnIkJointFromJoint(root, MObject::kNullObj, visited);
	return rval;
}

/// creation of a MFnJoint from a Joint member
/// We assume that each Joint has an unique identifier based on its name.
void createMFnIkJointFromJoint(Joint* joint, MFnIkJoint& father_mfnjoint, map<string, bool>& visited)
{
	visited[joint->_name] = true;
	MFnIkJoint child_mfnjoint;
	child_mfnjoint.create(father_mfnjoint);
	// TODO : init all values of the MFnIkJoint
	vector<Joint*>::iterator joint_children;
	for (joint_children = joint->_children.begin(); joint_children != joint->end(); joint_children++) {
		it = visited.find(*joint_children->_name);
		if (it != visited.end()) {
			createMFnIkJointFromJoint(*joint_children, child_mfnjoint, visited);
		}
	}

}

// The currently recognised primitives.
const char* primitiveStrings[] = {
	"nurbsSphere",
	"nurbsCone",
	"nurbsCylinder",
};
const unsigned numPrimitives = sizeof(primitiveStrings) / sizeof(char*);

// Corresponding commands to create the primitives
const char* primitiveCommands[] = {
	"sphere",
	"cone",
	"cylinder",
};

//The writer simply goes gathers all objects from the scene.
//We will check if the object has a transform, if so, we will check
//if it's either a nurbsSphere, nurbsCone or nurbsCylinder.  If so,
//we will write it out.
MStatus BvhTranslator::writer(const MFileObject& file,
	const MString& options,
	MPxFileTranslator::FileAccessMode mode)
{
	MStatus status;
	bool showPositions = false;
	unsigned int  i;
	const MString fname = file.fullName();

	ofstream newf(fname.asChar(), ios::out);
	if (!newf) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}
	newf.setf(ios::unitbuf);

	if (options.length() > 0) {
		// Start parsing.
		MStringArray optionList;
		MStringArray theOption;
		options.split(';', optionList);    // break out all the options.

		for (i = 0; i < optionList.length(); ++i) {
			theOption.clear();
			optionList[i].split('=', theOption);
			if (theOption[0] == MString("showPositions") &&
				theOption.length() > 1) {
				if (theOption[1].asInt() > 0) {
					showPositions = true;
				}
				else {
					showPositions = false;
				}
			}
		}
	}

	// output our magic number
	newf << "<LEP>\n";

	MItDag dagIterator(MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if (!status) {
		status.perror("Failure in DAG iterator setup");
		return MS::kFailure;
	}

	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);
	MItSelectionList selIterator(selection, MFn::kDagNode);

	bool done = false;
	while (true)
	{
		MObject currentNode;
		switch (mode)
		{
		case MPxFileTranslator::kSaveAccessMode:
		case MPxFileTranslator::kExportAccessMode:
			if (dagIterator.isDone())
				done = true;
			else {
				currentNode = dagIterator.item();
				dagIterator.next();
			}
			break;
		case MPxFileTranslator::kExportActiveAccessMode:
			if (selIterator.isDone())
				done = true;
			else {
				selIterator.getDependNode(currentNode);
				selIterator.next();
			}
			break;
		default:
			cerr << "Unrecognized write mode: " << mode << endl;
			break;
		}
		if (done)
			break;

		//We only care about nodes that are transforms
		MFnTransform dagNode(currentNode, &status);
		if (status == MS::kSuccess)
		{
			MString nodeNameNoNamespace = MNamespace::stripNamespaceFromName(dagNode.name());
			for (i = 0; i < numPrimitives; ++i) {
				if (nodeNameNoNamespace.indexW(primitiveStrings[i]) >= 0) {
					// This is a node we support
					newf << primitiveCommands[i] << " -n " << nodeNameNoNamespace << endl;
					if (showPositions) {
						MVector pos;
						pos = dagNode.getTranslation(MSpace::kObject);
						newf << "move " << pos.x << " " << pos.y << " " << pos.z << endl;
					}
				}
			}
		}//if (status == MS::kSuccess)
	}//while loop

	newf.close();
	return MS::kSuccess;
}

// Whenever Maya needs to know the preferred extension of this file format,
// it calls this method. For example, if the user tries to save a file called
// "test" using the Save As dialog, Maya will call this method and actually
// save it as "test.lep". Note that the period should *not* be included in
// the extension.
MString BvhTranslator::defaultExtension() const
{
	return "lep";
}


//This method is pretty simple, maya will call this function
//to make sure it is really a file from our translator.
//To make sure, we have a little magic number and we verify against it.
MPxFileTranslator::MFileKind BvhTranslator::identifyFile(
	const MFileObject& fileName,
	const char* buffer,
	short size) const
{
	// Check the buffer for the "LEP" magic number, the
	// string "<LEP>\n"

	MFileKind rval = kNotMyFileType;

	if ((size >= (short)magic.length()) &&
		(0 == strncmp(buffer, magic.asChar(), magic.length())))
	{
		rval = kIsMyFileType;
	}
	return rval;
}

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

	// Register the translator with the system
	// The last boolean in this method is very important.
	// It should be set to true if the reader method in the derived class
	// intends to issue MEL commands via the MGlobal::executeCommand 
	// method.  Setting this to true will slow down the creation of
	// new objects, but allows MEL commands other than those that are
	// part of the Maya Ascii file format to function correctly.
	status = plugin.registerFileTranslator("Lep",
		"BvhTranslator.rgb",
		BvhTranslator::creator,
		"BvhTranslatorOpts",
		"showPositions=1",
		true);
	if (!status)
	{
		status.perror("registerFileTranslator");
		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterFileTranslator("Lep");
	if (!status)
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	return status;
}
