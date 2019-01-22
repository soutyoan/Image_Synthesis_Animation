#include "bvhTranslator.h"

///Creates one instance of the BvhTranslator
void* BvhTranslator::creator()
{
	return new BvhTranslator();
}

// Initialize our magic string
MString const BvhTranslator::magic("HIERARCHY");

/**
 * Reader bvh file function using parsers to create Joint objects
 * @param  file    input file to be read
 * @param  options options
 * @param  mode    mode reading
 * @return         MS::kSuccess if the reading process ends well
 * 				   MS::kFailure otherwise
 */
MStatus BvhTranslator::reader(const MFileObject& file,
	const MString& options,
	MPxFileTranslator::FileAccessMode mode)
{
	const MString fname = file.fullName();

	MStatus rval(MS::kSuccess);

	ifstream inputfile(fname.asChar(), ios::in);
	if (!inputfile) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}

	string buf;
	inputfile >> buf;
	if (buf != kHierarchy) {
		MGlobal::displayError("Could not parse the file : missing HIERARCHY header\n");
		return MS::kFailure;
	}

	rval = parser_hierarchy(file);
	inputfile.close();

	return rval;
}


/**
 * Bvh parser for HIERARCHY section
 * @param  file input file
 * @return      MS::kSuccess if parsing, MS::kFailure otherwise
 */
MStatus BvhTranslator::parser_hierarchy(ifstream& file)
{
	string buf;
	MStatus rval;
	if (file.good()) {
		file >> buf;
		if (buf != kRoot) {
			displayError("Could not parse the file : ROOT header missing\n");
			return MS::kFailure;
		}
		MFnIkJoint rootJoint;
		rval = parser_joint(file, kNullObj, rootJoint);
	}
	if (file.good()) {
		file >> buf;
		if (buf!=kMotion) {
			displayError("Could not parse the file : MOTION header missing\n");
			return MS::kFailure;
		}
		rval = parser_motion(file);
	}
	return MS::kSuccess;
}


Mstatus BvhTranslator::parser_joint(ifstream& file, MFnIkJoint& parent, MFnIkJoint& current)
{
	string buf;
	MStatus rval;
	// linking current to its parent+name setting
	current.create(parent);
	file >> buf;
	current.setName(MString(buf));
	displayInfo("Creating Joint. \n");
	file >> buf; // parsing '{'

	// offset parsing
	file >> buf;
	if (buf!=kOffset) {
		displayError("Could not parse file : missing OFFSET keyword\n");
		return MS::kFailure;


	}
	for (int _i=0; _i<3; _i++) {
		file >> buf;
		// TODO : set offset of current joint
	}

	// channel parsing
	file >> buf;
	if (buf!=kChannels) {
		diplayError("Could not parse file : missing CHANNELS keyword\n");
	}
	file >> buf;
	int _nb = atoi(buf);
	for (int _i=0; _i<_nb; _i++) {
		file >> buf;
		// TODO : deal with channels values
	}

	// Children joints parsing
	while (file.good()) {
		file >> buf;
		if (buf==kJoint) { // joint
			MFnIkJoint child;
			rval = parser_joint(file, current, child);
		} else if (buf==kEnd) { // end of site
			file >> buf; // parsing '{'
			MFnIkJoint child;
			child.create(current);
			child.setName(MString(kEndSite));
			file >> buf >> buf; // parsing 'Site {'
			file >> buf;
			if (buf!=kOffset) {
				displayError("Could not parse file : missing OFFSET keyword\n");
				return MS::kFailure;
			}
			for (int _i=0; _i<3; _i++) {
				file >> buf;
				// TODO : set offest of child joint
			}
			file >> buf; // parsing '}' of end site
		} else if (buf=='}') { // end of current joint
			return MS::kSuccess;
		}
	}
}

MStatus BvhTranslator::parser_motion(ifstream& file)
{
	string buf;
	file >> buf;
	if (buf!=kMotion) {
		displayError("Could not parse file : missing MOTION keyword\n");
		return MS;kFailure;
	}
	// TODO : the frames parsing
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



// Whenever Maya needs to know the preferred extension of this file format,
// it calls this method. For example, if the user tries to save a file called
// "test" using the Save As dialog, Maya will call this method and actually
// save it as "test.lep". Note that the period should *not* be included in
// the extension.
MString BvhTranslator::defaultExtension() const
{
	return "bvh";
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
