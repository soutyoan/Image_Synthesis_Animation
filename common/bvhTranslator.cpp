#include "bvhTranslator.h"
Joint* _root = NULL;

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
		MGlobal::MGlobal::displayError("Could not parse the file : missing HIERARCHY header\n");
		return MS::kFailure;
	}


	// Creating Joint root object
	_root = Joint::create("ROOT", 0, 0, 0, NULL);
	rval = parser_hierarchy(inputfile);
	inputfile.close();

	// Create MAYA Joint
	rval = Joint_to_MAYA(_root, MObject::kNullObj);

	return rval;
}


MStatus BvhTranslator::Joint_to_MAYA(Joint* joint, MObject& Mparent)
{
	MStatus rval;
	MFnIkJoint Mjoint;
	MObject created = Mjoint.create(Mparent);

	//return MS::kSuccess;
	const char *cname = joint->_name.c_str();
	//return MS::kSuccess;
	Mjoint.setName(MString(cname));
	const double offs[3] = { joint->_offX, joint->_offY, joint->_offZ };
	Mjoint.setTranslation(MVector(offs), MSpace::kTransform);

	// MOTION PART 
	if (Mparent == MObject::kNullObj) {
		MString attrNameX("translateX");
		MString attrNameY("translateY");
		MString attrNameZ("translateZ");

		MDagPath mObject;
		Mjoint.getPath(mObject);

		// ATTX 
		const MObject attrX = Mjoint.attribute(attrNameX, &rval);
		if (MS::kSuccess != rval) {
			cerr << "Failure to find attribute\n";
		}

		MFnAnimCurve acFnSetX;
		acFnSetX.create(mObject.transform(), attrX, NULL, &rval);

		if (MS::kSuccess != rval) {
			cerr << "Failure creating MFnAnimCurve function set (translateX)\n";
		}
		std::vector<double> translationsJointX = joint->_dofs[0]._values;
		
		//ENDATTX 

		// ATTY 
		const MObject attrY = Mjoint.attribute(attrNameY, &rval);
		if (MS::kSuccess != rval) {
			cerr << "Failure to find attribute\n";
		}

		MFnAnimCurve acFnSetY;
		acFnSetY.create(mObject.transform(), attrY, NULL, &rval);

		if (MS::kSuccess != rval) {
			cerr << "Failure creating MFnAnimCurve function set (translateX)\n";
		}

		std::vector<double> translationsJointY = joint->_dofs[1]._values;

		//ENDATTY 

		// ATTX 
		const MObject attrZ = Mjoint.attribute(attrNameZ, &rval);
		if (MS::kSuccess != rval) {
			cerr << "Failure to find attribute\n";
		}

		MFnAnimCurve acFnSetZ;
		acFnSetZ.create(mObject.transform(), attrZ, NULL, &rval);

		if (MS::kSuccess != rval) {
			cerr << "Failure creating MFnAnimCurve function set (translateX)\n";
		}
		std::vector<double> translationsJointZ = joint->_dofs[2]._values;

		for (int i = 0; i < translationsJointX.size(); i++) {

			MTime tm((double)i, MTime::kFilm);
			if ((MS::kSuccess != acFnSetX.addKeyframe(tm, translationsJointX[i])) ||
				(MS::kSuccess != acFnSetY.addKeyframe(tm, translationsJointY[i])) ||
				(MS::kSuccess != acFnSetZ.addKeyframe(tm, translationsJointZ[i]))) {
				cerr << "Error setting the keyframe\n";
			}
		}
		//ENDATTX 
		MGlobal::displayError("ANIMATION ROOT CREATED\n");
	}


	//END MOTION PART

	vector<Joint*>::iterator children;
	for (children = joint->_children.begin(); children != joint->_children.end(); children++) {
		rval = Joint_to_MAYA(*children, created);
	}
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
	file >> buf;
	if (buf != kRoot) {
		MGlobal::displayError("Could not parse the file : ROOT header missing\n");
		return MS::kFailure;
	}
	rval = parser_joint(file, buf);
	rval = parser_motion(file, buf);
	return MS::kSuccess;
}


MStatus BvhTranslator::parser_joint(ifstream& file, string &buf)
{
	string name;
	double _offx; double _offy; double _offz;
	MStatus rval;
	Joint *parent = NULL;
	Joint *current = _root;
	//return MStatus::kSuccess;

	// Creating the root Joint //
	rval = parser_offset(file, name, _offx, _offy, _offz, buf);
	*current = *Joint::create(name, _offx, _offy, _offz, parent);
	
	
	rval = parser_channels(file, current, buf);

	// Children joints parsing
	while (buf!=kMotion) {
		MGlobal::displayError("loop\n");
		file >> buf;
		if (buf==kJoint) { // joint
			parent = current;
			rval = parser_offset(file, name, _offx, _offy, _offz, buf);
			current = Joint::create(name, _offx, _offy, _offz, parent);
			rval = parser_channels(file, current, buf);

		} else if (buf==kEnd) { // end of site
			parent = current;
			rval = parser_offset(file, name, _offx, _offy, _offz, buf);
			current = Joint::create(name, _offx, _offy, _offz, parent);
			//file >> buf; // parsing '}' of End Site
		} else if (buf=="}") { // end of current joint
			MGlobal::displayInfo("} case\n");
			current = current->_parent;
		}
		else if (buf == kMotion) {
			MGlobal::displayInfo("MOT \n");
		}
		else {
			MGlobal::displayError("Could not parse file : unknown token\n");
			MGlobal::displayError(MString(buf.c_str()));
			MGlobal::displayError(MString(current->_name.c_str()));
			return MS::kFailure;
		}
	}
	MGlobal::displayInfo("JAI TOUT LU\n");
	return MS::kSuccess;
}

MStatus BvhTranslator::parser_offset(ifstream& file, string& name, double& _offx, double& _offy, double& _offz, string& buf)
{
	file >> name;
	MGlobal::displayInfo("Creating Joint. \n");
	file >> buf; // parsing '{'

	file >> buf; // parsing 'OFFSET'
	if (buf != kOffset) {
		MGlobal::displayError("Could not parse file : missing OFFSET keyword\n");
		return MS::kFailure;
	}
	try {
		file >> _offx >> _offy >> _offz;
	}
	catch (...) {
		MGlobal::displayError("Could not parse Offset of current Joint\n");
		return MS::kFailure;
	}
	return MS::kSuccess;
}

MStatus BvhTranslator::parser_channels(ifstream& file, Joint* current, string& buf)
{
	
	file >> buf; // parsing CHANNELS

	
	
	if (buf != kChannels) {
		MGlobal::displayError("Could not parse file : missing CHANNELS keyword\n");
	}
	file >> buf;
	int _nb = stoi(buf);
	for (int _i = 0; _i < _nb; _i++) {
		file >> buf;
		AnimCurve _currAnim;
		_currAnim.name = buf;
		current->_dofs.push_back(_currAnim);
	}
	MGlobal::displayInfo("Chanel\n");
	return MStatus::kSuccess;
}

MStatus parseRec(ifstream& file, string &buf, Joint *current) {
	if (current == NULL) {
		MGlobal::displayError(MString("Current is Null not possible"));
		return MStatus::kFailure;

	}
	int numbr_channels = current->_dofs.size(); // I initialize myself
	stringstream ss;
	ss << numbr_channels;
	string str = ss.str();
	for (int i = 0; i < numbr_channels; i++) {
		current->_dofs[i]._values.push_back(stod(buf));
		file >> buf;
	}

	

	vector<Joint*>::iterator children;
	for (children = current->_children.begin(); children != current->_children.end(); children++) {
		parseRec(file, buf, *children);
	}

	return MStatus::kSuccess;

	

}

MStatus BvhTranslator::parser_motion(ifstream& file, string &buf)
{
	if (buf!=kMotion) {
		MGlobal::displayError(MString("Could not parse file : missing MOTION keyword\n"));
		return MS::kFailure;
	}
	file >> buf;

	file >> buf;

	int nbr_frames = stoi(buf);
	file >> buf;
	file >> buf;
	file >> buf;
	file >> buf;

	for (int frame = 0; frame < nbr_frames; frame++) {
		parseRec(file, buf, _root);
	}
	return MS::kSuccess;
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
		"lepTranslator.rgb",
		BvhTranslator::creator,
		"lepTranslatorOpts",
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