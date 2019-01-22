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
	string buf;

	ifstream inputfile(fname.asChar(), ios::in);
	if (!inputfile) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}

	rval = parser_hierarchy(file);
	inputfile.close();

	return rval;
}

MStatus BvhTranslator::parser_hierarchy(ifstream& file)
{
	// TODO : parser for hierarchy
}

Mstatus BvhTranslator::parser_joint(ifstream& file, MFnObject& parent, MFnObject& current)
{
	// TODO : parser for a joint
}

MStatus BvhTranslator::parser_motion(ifstream& file)
{
	// TODO : parser for motion part
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
